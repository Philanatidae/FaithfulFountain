/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-asset-builder/MetalShaderFunctionBuildStep.hpp>

#include <ff-asset-builder/AssetBuilder.hpp>

#include <tinyformat/tinyformat.h>
#include <ff/Console.hpp>

#include <ff/io/BinaryMemory.hpp>
#include <ff/io/StreamBinaryWriter.hpp>

#include <spirv_cross.hpp>
#include <spirv_msl.hpp>

#include <fstream>

#include "TargetConditionals.h"

namespace ff {
    MetalShaderFunctionBuildStep::MetalShaderFunctionBuildStep(const std::string& name)
        :_name(name) {
    }
    MetalShaderFunctionBuildStep::~MetalShaderFunctionBuildStep() {
    }

    std::string MetalShaderFunctionBuildStep::getType() const {
        return "MetalShaderFunctionBuildStep";
    }

    const std::string& MetalShaderFunctionBuildStep::getName() const {
        return _name;
    }
    
    const std::filesystem::path& MetalShaderFunctionBuildStep::getAirPath() const {
        return _airPath;
    }
    const std::vector<ShaderFunctionBinding>& MetalShaderFunctionBuildStep::getBindings() const {
        return _bindings;
    }

    void MetalShaderFunctionBuildStep::build(AssetBuilder* assetBuilder) {
        FF_ASSERT(getInputs().size() == 1, "MetalShaderFunctionBuildStep must have exactly one input.");

        // Compile to SPIR-V
        // @todo Use glslang directly within ff-asset-builder
        std::filesystem::path spirvPath = assetBuilder->getObjectDir()/tinyformat::format("%s_metal.spv", getName());
        std::string glslangCMD = tinyformat::format("glslangValidator -V %s -o %s -e %s --source-entrypoint main",
            getInputs()[0].string(),
            spirvPath,
            getName());
        FF_ASSERT(assetBuilder->runCommand(glslangCMD), "Generating SPIR-V for `%s` failed.", getName());

        // Load SPIR-V into memory
        std::vector<uint32_t> spirvBinary;
        {
            std::ifstream spirvStream(spirvPath, std::ifstream::binary);
            spirvStream.seekg(0, std::ios::end);
            size_t spirvSize = spirvStream.tellg();
            spirvStream.seekg(0, std::ios::beg);

            spirvBinary.resize(spirvSize / sizeof(uint32_t));

            spirvStream.read((char*)spirvBinary.data(), spirvSize);
        }
        spirv_cross::CompilerMSL msl(std::move(spirvBinary));
        spirv_cross::CompilerMSL::Options options;
        options.enable_decoration_binding = true;
        options.set_msl_version(2); // MSL 2 required for uniform texture arrays
        msl.set_msl_options(options);
        
        // Get info for metadata
        auto resources = msl.get_shader_resources();
        for(auto& resource : resources.sampled_images) {
            uint32_t binding = msl.get_decoration(resource.id, spv::DecorationBinding);

            spirv_cross::SPIRType type = msl.get_type(resource.type_id);
            if(type.array.size() > 0) {
                FF_ASSERT(type.array.size() == 1, "Unknown case.");
                int imageArrayCount = (int)type.array[0];
                // This is an array; in GL and Metal, they simply take the binding for
                // element 0 and the other elements are binding + i.
                for(int i = 0; i < imageArrayCount; i++) {
                    _bindings.push_back(ShaderFunctionBinding(tinyformat::format("%s[%s]", resource.name, i),
                        binding + i,
                        ShaderFunctionBindingType::Texture));
                }
            } else {
                _bindings.push_back(ShaderFunctionBinding(resource.name,
                    binding,
                    ShaderFunctionBindingType::Texture));
            }
        }
        for(auto& resource : resources.uniform_buffers) {
            uint32_t binding = msl.get_decoration(resource.id, spv::DecorationBinding);
            _bindings.push_back(ShaderFunctionBinding(resource.name,
                binding,
                ShaderFunctionBindingType::UniformBuffer));
        }

        // Generate MSL
        std::string mslPath = (assetBuilder->getObjectDir()/tinyformat::format("%s.metal", getName())).string();
        std::string mslSource = msl.compile();
        FF_ASSERT(!mslSource.empty(), "Generating MSL for `%s` failed.", getName());
        {
            StreamBinaryWriter mslWriter(std::make_shared<std::ofstream>(mslPath, std::ofstream::binary));
            mslWriter.write((uint8_t*)mslSource.c_str(), mslSource.size());
        }

        bool isRequiredToolkit = false;
#ifdef __MAC_OS_X_VERSION_MAX_ALLOWED
#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 13
        isRequiredToolkit = true;
#endif
#endif

        if(!isRequiredToolkit) {
            assetBuilder->invalidateForDistribution();
        }

        // Generate AIR
        _airPath = assetBuilder->getObjectDir()/tinyformat::format("%s.air", getName());
        std::string flags = "";
        if(!assetBuilder->isProductionBuild()) {
            flags += " -gline-tables-only";

#if __MAC_OS_X_VERSION_MAX_ALLOWED >= 13
            flags += " -frecord-sources=flat";
#endif
        }
        std::string metalPlatform;
        // @todo Choose Metal version
        // @todo Put flags in a central place so we can share it between the function build step and library build target
        switch(assetBuilder->getPlatformTarget()) {
            case PlatformTarget::IOS:
                metalPlatform = "iphoneos";
                flags += " -mios-version-min=13.0 -target air64-apple-ios13.0";
                break;
            case PlatformTarget::IOS_SIMULATOR:
                metalPlatform = "iphonesimulator";
                flags += " -mios-version-min=13.0 -target air64-apple-ios13.0";
                break;
            case PlatformTarget::MACOS:
                metalPlatform = "macosx";
                flags += " -mmacosx-version-min=10.13 -target air64-apple-macosx10.13";
                break;
            default:
                FF_CONSOLE_FATAL("Can not compile Metal shaders for platform `%s`.",
                    convertPlatformTargetToString(assetBuilder->getPlatformTarget()));
        }
        FF_ASSERT(assetBuilder->runCommand(tinyformat::format("xcrun -sdk %s metal %s -c \"%s\" -o %s",
            metalPlatform,
            flags,
            mslPath,
            _airPath.string())), "Shader `%s` did not compile into intermediate representation successfully.", getName());
    }
}
