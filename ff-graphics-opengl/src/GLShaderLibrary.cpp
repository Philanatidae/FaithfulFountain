/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLShaderLibrary.hpp>

#include <ff-graphics-opengl/GLShaderFunction.hpp>

#include <ff/graphics/ShaderFunctionBindingType.hpp>

#include <memory>
#include <utility>

#include <ff/Locator.hpp>

#include <ff/io/BinaryMemory.hpp>

namespace ff {
    GLShaderLibrary::GLShaderLibrary(ff::IAssetBundle& assetBundle, const nlohmann::json& assetObject)
        :_graphicsDevice(static_cast<GLGraphicsDevice*>(&Locator::getGraphicsDevice())) {
        FF_ASSET_TYPE_CHECK(assetObject, "GLShaderLibrary");

        std::unordered_map<std::string, std::unique_ptr<GLShaderFunction>> _functionsCache;

        for(auto shaderObject : assetObject["shaders"]) {
            FF_ASSERT(!shaderObject["name"].is_null(), "Missing key `name` in shader for library `%s`.", assetObject["name"]);
            FF_ASSERT(!shaderObject["vertex-function"].is_null(), "Missing key `vertex-function` in shader for library `%s`.", assetObject["name"]);
            FF_ASSERT(!shaderObject["fragment-function"].is_null(), "Missing key `fragment-function` in shader for library `%s`.", assetObject["name"]);

            GLShaderFunction& vertexFunction = getOrCreateFunctionFromCache(shaderObject["vertex-function"], GL_VERTEX_SHADER, assetBundle, _functionsCache);
            GLShaderFunction& fragmentFunction = getOrCreateFunctionFromCache(shaderObject["fragment-function"], GL_FRAGMENT_SHADER, assetBundle, _functionsCache);

            std::vector<std::pair<std::string, int>> _textureBindings;
            std::vector<std::pair<std::string, int>> _uniformBufferBindings;
            for(auto bindingObject : shaderObject["bindings"]) {
                FF_ASSERT(!bindingObject["name"].is_null(), "Missing key `name` in binding for shader `%s` in library `%s`.", shaderObject["name"], assetObject["name"]);
                FF_ASSERT(!bindingObject["type"].is_null(), "Missing key `type` in binding for shader `%s` in library `%s`.", shaderObject["name"], assetObject["name"]);
                FF_ASSERT(!bindingObject["binding"].is_null(), "Missing key `binding` in binding for shader `%s` in library `%s`.", shaderObject["name"], assetObject["name"]);

                auto bindingPair = std::make_pair(bindingObject["name"], bindingObject["binding"]);

                ShaderFunctionBindingType bindingType = convertStringToShaderFunctionBindingType(bindingObject["type"]);
                switch(bindingType) {
                case ShaderFunctionBindingType::Texture:
                    _textureBindings.push_back(bindingPair);
                    break;
                case ShaderFunctionBindingType::UniformBuffer:
                    _uniformBufferBindings.push_back(bindingPair);
                    break;
                }
            }

            FF_ASSERT(_shaders.find(shaderObject["name"]) == _shaders.end(), "Shader `%s` already exists.", shaderObject["name"]);
            _shaders.emplace(shaderObject["name"], std::make_shared<GLShader>(_graphicsDevice,
                shaderObject["name"],
                vertexFunction,
                fragmentFunction,
                _textureBindings,
                _uniformBufferBindings));
        }
    }
    GLShaderLibrary::~GLShaderLibrary() {
    }

    std::shared_ptr<GLShader> GLShaderLibrary::getShader(const std::string& name) const {
        auto it = _shaders.find(name);
        FF_ASSERT(it != _shaders.end(), "Invalid shader name: `%s`.", name);
        return it->second;
    }

    GLShaderFunction& GLShaderLibrary::getOrCreateFunctionFromCache(const std::string& function, const GLenum& shaderType, IAssetBundle& assetBundle, std::unordered_map<std::string, std::unique_ptr<GLShaderFunction>>& cache) {
        if(cache.find(function) == cache.end()) {
            std::string suffix = _graphicsDevice->isGLES() ? "-GLES" : "-GL";
            BinaryMemory memory(*assetBundle.getAssetReader(tinyformat::format("%s%s", function, suffix)));
            std::string functionSource = memory.toString();
            cache.emplace(function, std::make_unique<GLShaderFunction>(_graphicsDevice, function, shaderType, functionSource));
        }

        return *cache[function];
    }
}
