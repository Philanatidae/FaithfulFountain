/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-asset-builder/GLShaderLibraryBuildTarget.hpp>

#include <ff-asset-builder/GLShaderFunctionBuildStep.hpp>

#include <ff-asset-builder/AssetBuilder.hpp>

#include <tinyformat/tinyformat.h>
#include <unordered_set>

#include <ff/util/OS.hpp>

#include <ff/Console.hpp>

namespace ff {
    GLShaderLibraryBuildTarget::GLShaderLibraryBuildTarget(const std::string& name,
        const std::vector<ShaderDefinition>& shaders,
        const bool& isES)
        :_name(name),_shaders(shaders),
        _isES(isES) {
    }
    GLShaderLibraryBuildTarget::~GLShaderLibraryBuildTarget() {
    }

    std::string GLShaderLibraryBuildTarget::getType() const {
        return "GLShaderLibrary";
    }
    std::string GLShaderLibraryBuildTarget::getName() const {
        return _name;
    }
    uint32_t GLShaderLibraryBuildTarget::getFlags() const {
        return 0;
    }

    std::vector<std::filesystem::path> GLShaderLibraryBuildTarget::getOutputs(AssetBuilder* builder) const {
        std::unordered_set<std::string> funcs;
        std::vector<std::filesystem::path> outputs;
        for(const auto& shader : _shaders) {
            if(funcs.find(shader.vertexFunction) == funcs.end()) {
                funcs.insert(shader.vertexFunction);
                outputs.push_back(builder->getTargetDir()/shader.vertexFunction);
            }
            if(funcs.find(shader.fragmentFunction) == funcs.end()) {
                funcs.insert(shader.fragmentFunction);
                outputs.push_back(builder->getTargetDir()/shader.fragmentFunction);
            }
        }
        return outputs;
    }

    const std::vector<ShaderDefinition>& GLShaderLibraryBuildTarget::getShaders() const {
        return _shaders;
    }
    const bool& GLShaderLibraryBuildTarget::isES() const {
        return _isES;
    }

    void GLShaderLibraryBuildTarget::build(AssetBuilder* builder) {
        _shaderMetadata.clear();

        for(const auto& shader : _shaders) {
            nlohmann::json shaderObject;
            shaderObject["name"] = shader.name;
            shaderObject["bindings"] = nlohmann::json::array();

            std::shared_ptr<GLShaderFunctionBuildStep> vertexFunction = std::static_pointer_cast<GLShaderFunctionBuildStep>(builder->buildAndGetBuildStep(shader.vertexFunction));
            FF_ASSERT(vertexFunction != nullptr, "Could not get vertex function build step: `%s`", shader.vertexFunction);
            std::shared_ptr<GLShaderFunctionBuildStep> fragmentFunction = std::static_pointer_cast<GLShaderFunctionBuildStep>(builder->buildAndGetBuildStep(shader.fragmentFunction));
            FF_ASSERT(fragmentFunction != nullptr, "Could not get fragment function build step: `%s`", shader.fragmentFunction);

            shaderObject["vertex-function"] = vertexFunction->getName();
            shaderObject["fragment-function"] = fragmentFunction->getName();

            std::filesystem::path vertexFuncTargetPath = builder->getTargetDir()/shader.vertexFunction;
            copyFileIfNewer(vertexFunction->getGLSLPath(), vertexFuncTargetPath);
            for(const auto& binding : vertexFunction->getBindings()) {
                nlohmann::json bindingObj;
                bindingObj["name"] = binding.name;
                bindingObj["binding"] = binding.binding;
                bindingObj["type"] = convertShaderFunctionBindingTypeToString(binding.type);
                shaderObject["bindings"].push_back(bindingObj);
            }
            std::filesystem::path fragmentFuncTargetPath = builder->getTargetDir()/shader.fragmentFunction;
            copyFileIfNewer(fragmentFunction->getGLSLPath(), fragmentFuncTargetPath);
            for(const auto& binding : fragmentFunction->getBindings()) {
                nlohmann::json bindingObj;
                bindingObj["name"] = binding.name;
                bindingObj["binding"] = binding.binding;
                bindingObj["type"] = convertShaderFunctionBindingTypeToString(binding.type);
                shaderObject["bindings"].push_back(bindingObj);
            }

            _shaderMetadata.push_back(shaderObject);
        }
    }
    void GLShaderLibraryBuildTarget::populateMetadata(nlohmann::json& targetObject) {
        auto& shadersArray = targetObject["shaders"] = nlohmann::json::array();

        for(const auto& shader : _shaderMetadata) {
            shadersArray.push_back(shader);
        }
    }
}
