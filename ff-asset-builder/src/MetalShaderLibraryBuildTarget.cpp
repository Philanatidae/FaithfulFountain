/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-asset-builder/MetalShaderLibraryBuildTarget.hpp>

#include <ff-asset-builder/AssetBuilder.hpp>

#include <ff-asset-builder/MetalShaderFunctionBuildStep.hpp>
#include <tinyformat/tinyformat.h>
#include <ff/Console.hpp>

#include <unordered_set>

namespace ff {
    MetalShaderLibraryBuildTarget::MetalShaderLibraryBuildTarget(const std::string& name,
        const std::vector<ShaderDefinition>& shaders)
        :_name(name),_shaders(shaders) {
    }
    MetalShaderLibraryBuildTarget::~MetalShaderLibraryBuildTarget() {
    }

    std::string MetalShaderLibraryBuildTarget::getType() const {
        return "MetalShaderLibrary";
    }
    std::string MetalShaderLibraryBuildTarget::getName() const {
        return _name;
    }
    uint32_t MetalShaderLibraryBuildTarget::getFlags() const {
        return 0;
    }

    std::vector<std::filesystem::path> MetalShaderLibraryBuildTarget::getOutputs(AssetBuilder* builder) const {
        //if(builder->isProductionBuild()) {
            return {
                builder->getTargetDir()/getName()
            };
        // } else {
            // return {
                // builder->getTargetDir()/getName(),
                // builder->getTargetDir()/(getName() + ".metallibsym")
            // };
        // }
    }

    const std::vector<ShaderDefinition>& MetalShaderLibraryBuildTarget::getShaders() const {
        return _shaders;
    }

    void MetalShaderLibraryBuildTarget::build(AssetBuilder* builder) {
        _shaderMetadata.clear();

        std::string metalPlatform;
        switch(builder->getPlatformTarget()) {
            case PlatformTarget::IOS:
                metalPlatform = "iphoneos";
                break;
            case PlatformTarget::IOS_SIMULATOR:
                metalPlatform = "iphonesimulator";
                break;
            case PlatformTarget::MACOS:
                metalPlatform = "macosx";
                break;
            default:
                FF_CONSOLE_FATAL("Can not compile Metal shaders for platform `%s`.",
                    convertPlatformTargetToString(builder->getPlatformTarget()));
        }

        std::string airPathsCombined;
        std::unordered_set<std::string> addedFunctions;
        for (const auto& shader : _shaders) {
            std::shared_ptr<MetalShaderFunctionBuildStep> vertexFunction = std::static_pointer_cast<MetalShaderFunctionBuildStep>(builder->buildAndGetBuildStep(shader.vertexFunction));
            FF_ASSERT(vertexFunction, "Could not find vertex function `%s`.", shader.vertexFunction);
            if(!addedFunctions.count(vertexFunction->getName())) {
                addedFunctions.insert(vertexFunction->getName());
                airPathsCombined += tinyformat::format("%s ", vertexFunction->getAirPath().string());
            }
            std::shared_ptr<MetalShaderFunctionBuildStep> fragmentFunction = std::static_pointer_cast<MetalShaderFunctionBuildStep>(builder->buildAndGetBuildStep(shader.fragmentFunction));
            FF_ASSERT(fragmentFunction, "Could not find fragment function `%s`.", shader.fragmentFunction);
            if(!addedFunctions.count(fragmentFunction->getName())) {
                addedFunctions.insert(fragmentFunction->getName());
                airPathsCombined += tinyformat::format("%s ", fragmentFunction->getAirPath().string());
            }

            // Generate metadata
            nlohmann::json shaderMetadata = nlohmann::json::object();
            shaderMetadata["name"] = shader.name;
            shaderMetadata["vertex-function"] = vertexFunction->getName();
            shaderMetadata["fragment-function"] = fragmentFunction->getName();
            shaderMetadata["bindings"] = nlohmann::json::array();

            std::unordered_map<int, nlohmann::json> imageBindingObjects;
            std::unordered_map<int, nlohmann::json> uniformBufferBindingObjects;

            for(const auto& binding : vertexFunction->getBindings()) {
                populateBindingMetadata(binding, imageBindingObjects, uniformBufferBindingObjects, "vertex");
            }
            for(const auto& binding : fragmentFunction->getBindings()) {
                populateBindingMetadata(binding, imageBindingObjects, uniformBufferBindingObjects, "fragment");
            }

            for(const auto& bindingPair : imageBindingObjects) {
                shaderMetadata["bindings"].push_back(bindingPair.second);
            }
            for(const auto& bindingPair : uniformBufferBindingObjects) {
                shaderMetadata["bindings"].push_back(bindingPair.second);
            }

            _shaderMetadata.push_back(shaderMetadata);
        }
        
        std::string flags = "";
        if(!builder->isProductionBuild()) {
            // https://developer.apple.com/documentation/metal/shader_libraries/generating_and_loading_a_metal_library_symbol_file?language=objc
            flags += " -frecord-sources=flat";
        }

        // @todo Choose Metal version
        // @todo Put flags in a central place so we can share it between the function build step and library build target
        switch(builder->getPlatformTarget()) {
            case PlatformTarget::IOS:
                metalPlatform = "iphoneos";
                flags += " -mios-version-min=13.0 -target air64-apple-ios13.0";
                break;
            case PlatformTarget::IOS_SIMULATOR:
                metalPlatform = "iphonesimulator";
                flags += " -mios-version-min=13.0 -target air64-apple-ios13.0-simulator";
                break;
            case PlatformTarget::MACOS:
                metalPlatform = "macosx";
                flags += " -mmacosx-version-min=10.13 -target air64-apple-macosx10.13";
                break;
            default:
                FF_CONSOLE_FATAL("Can not compile Metal shaders for platform `%s`.",
                    convertPlatformTargetToString(builder->getPlatformTarget()));
        }

        std::string command = tinyformat::format("xcrun -sdk %s metal %s %s -o %s",
            metalPlatform,
            flags,
            airPathsCombined,
            getOutputs(builder)[0].string());
        FF_ASSERT(builder->runCommand(command), "Failed to link Metal shaders for `%s`.", getName());

        /*if(!builder->isProductionBuild()) {
            flags = " -flat -remove-source";

            std::string command = tinyformat::format("xcrun -sdk %s metal-dsymutil %s %s -o %s",
                metalPlatform,
                flags,
                getOutputs(builder)[0].string(),
                getOutputs(builder)[1].string());
            FF_ASSERT(builder->runCommand(command), "Failed to link Metal shaders for `%s`.", getName());
        }*/
    }
    void MetalShaderLibraryBuildTarget::populateMetadata(nlohmann::json& targetObject) {
        targetObject["path"] = getName();
        targetObject["format"] = "binary";

        targetObject["shaders"] = nlohmann::json::array();
        for(const auto& shader : _shaderMetadata) {
            targetObject["shaders"].push_back(shader);
        }
    }

    void MetalShaderLibraryBuildTarget::populateBindingMetadata(const ShaderFunctionBinding& binding,
        std::unordered_map<int, nlohmann::json>& imageBindingObjects,
        std::unordered_map<int, nlohmann::json>& uniformBufferBindingObjects,
        char const* const stageName) {
        switch(binding.type) {
        case ShaderFunctionBindingType::Texture:
            if(!imageBindingObjects.count(binding.binding)) {
                auto bindingObject = nlohmann::json::object();
                bindingObject["type"] = convertShaderFunctionBindingTypeToString(binding.type);
                bindingObject["slot"] = binding.binding;
                bindingObject["vertex-attachment"] = false;
                bindingObject["fragment-attachment"] = false;

                imageBindingObjects.emplace(binding.binding, bindingObject);
            }
            imageBindingObjects[binding.binding][tinyformat::format("%s-attachment", stageName)] = true;
            break;
        case ShaderFunctionBindingType::UniformBuffer:
            if(!uniformBufferBindingObjects.count(binding.binding)) {
                auto bindingObject = nlohmann::json::object();
                bindingObject["type"] = convertShaderFunctionBindingTypeToString(binding.type);
                bindingObject["slot"] = binding.binding;
                bindingObject["vertex-attachment"] = false;
                bindingObject["fragment-attachment"] = false;

                uniformBufferBindingObjects.emplace(binding.binding, bindingObject);
            }
            uniformBufferBindingObjects[binding.binding][tinyformat::format("%s-attachment", stageName)] = true;
            break;
        default:
            FF_CONSOLE_FATAL("Unsupported binding type `%s`.", convertShaderFunctionBindingTypeToString(binding.type));
            break;
        }
    }
}
