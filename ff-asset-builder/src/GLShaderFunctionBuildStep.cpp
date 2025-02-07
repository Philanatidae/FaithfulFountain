/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-asset-builder/GLShaderFunctionBuildStep.hpp>

#include <ff-asset-builder/AssetBuilder.hpp>

#include <tinyformat/tinyformat.h>
#include <ff/Console.hpp>

#include <ff/io/BinaryMemory.hpp>
#include <ff/io/StreamBinaryWriter.hpp>

#include <spirv_cross.hpp>
#include <spirv_glsl.hpp>

#include <fstream>

namespace ff {
    GLShaderFunctionBuildStep::GLShaderFunctionBuildStep(const std::string& name,
        const bool& es)
        :_name(name),
        _es(es) {
    }
    GLShaderFunctionBuildStep::~GLShaderFunctionBuildStep() {
    }

    std::string GLShaderFunctionBuildStep::getType() const {
        return "GLShaderFunctionBuildStep";
    }

    const std::string& GLShaderFunctionBuildStep::getName() const {
        return _name;
    }
    const bool& GLShaderFunctionBuildStep::isES() const {
        return _es;
    }

    const std::filesystem::path& GLShaderFunctionBuildStep::getGLSLPath() const {
        return _glslPath;
    }
    const std::vector<ShaderFunctionBinding>& GLShaderFunctionBuildStep::getBindings() const {
        return _bindings;
    }

    void GLShaderFunctionBuildStep::build(AssetBuilder* assetBuilder) {
        FF_ASSERT(getInputs().size() == 1, "GLShaderFunctionBuildStep must have exactly one input.");

        const int glslVersion = isES() ? 300 : 400;

        // Compile to SPIR-V
        // @todo Use glslang directly within ff-asset-builder
        std::filesystem::path spirvPath = assetBuilder->getObjectDir()/tinyformat::format("%s_glsl%s%s.spv",
            getName(),
            glslVersion,
            isES() ? "_es" : "");
        std::string glslangCMD = tinyformat::format("glslangValidator -V %s %s -o %s -e %s --source-entrypoint main",
            getInputs()[0].string(),
            isES() ? "-DES=1" : "",
            spirvPath,
            getName());
        FF_ASSERT(assetBuilder->runCommand(glslangCMD), "Generating SPIR-V for `%s` failed.", getName());

        // Load SPIR-V into memory
        std::vector<uint32_t> spirvBinary;
        {
            std::ifstream spirvStream(spirvPath.string(), std::ifstream::binary);
            spirvStream.seekg(0, std::ios::end);
            size_t spirvSize = spirvStream.tellg();
            spirvStream.seekg(0, std::ios::beg);

            spirvBinary.resize(spirvSize / sizeof(uint32_t));

            spirvStream.read((char*)spirvBinary.data(), spirvSize);
        }
        spirv_cross::CompilerGLSL glsl(spirvBinary.data(), spirvBinary.size());
        spirv_cross::CompilerGLSL::Options options;
        options.es = isES();
        options.version = glslVersion;
        options.enable_420pack_extension = false;
        options.vulkan_semantics = false;

        // fixup_clipspace (from SPIRV-cross): "For GLSL targets, enabling this will
        // convert a shader which assumes [0, w] depth range (Vulkan/D3D/Metal) into
        // [-w, w] range."
        //
        // Faithful Fountain is designed such that shaders are written as if they
        // could be used in Direct3D directly. In fact, if HLSL transpilation was
        // better, we would be using HLSL instead of GLSL. Shaders are written as if they
        // were to be used in Vulkan/Metal/D3D.
        options.vertex.fixup_clipspace = true;

        glsl.set_common_options(options);

        // Get info for metadata
        auto resources = glsl.get_shader_resources();
        for(auto& resource : resources.sampled_images) {
            uint32_t binding = glsl.get_decoration(resource.id, spv::DecorationBinding);

            spirv_cross::SPIRType type = glsl.get_type(resource.type_id);
            if(type.array.size() > 0) {
                FF_ASSERT(type.array.size() == 1, "Unknown case.");
                int imageArrayCount = (int)type.array[0];
                // This is an array; in GL and Metal, they simply take the binding for
                // element 0 and the other elements are binding + i.
                for(int i = 0; i < imageArrayCount; i++) {
                    _bindings.push_back(ShaderFunctionBinding(tinyformat::format("%s[%s]",
                            resource.name, i),
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
            uint32_t binding = glsl.get_decoration(resource.id, spv::DecorationBinding);
            _bindings.push_back(ShaderFunctionBinding(resource.name,
                binding,
                ShaderFunctionBindingType::UniformBuffer));
        }

        // Generate GLSL
        _glslPath = assetBuilder->getObjectDir()/tinyformat::format("%s_glsl%s%s.glsl",
            getName(),
            glslVersion,
            isES() ? "_es" : "");
        std::string glslSource = glsl.compile();
        FF_ASSERT(!glslSource.empty(), "Failed to generate GLSL for `%s`.", getName());
        {
            StreamBinaryWriter glslWriter(std::make_shared<std::ofstream>(_glslPath, std::ios::binary));
            glslWriter.write((uint8_t*)glslSource.c_str(), glslSource.size());
        }
    }
}
