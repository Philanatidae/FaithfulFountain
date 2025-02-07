/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-metal/MetalShaderLibrary.hmm>

#include <ff/graphics/ShaderFunctionBindingType.hpp>

#include <ff-graphics-metal/MetalLibrary.hmm>

#include <ff/Locator.hpp>

#include <unordered_set>

namespace ff {
    MetalShaderLibrary::MetalShaderLibrary(ff::IAssetBundle& assetBundle, const nlohmann::json& assetObject) {
        FF_ASSET_TYPE_CHECK(assetObject, "MetalShaderLibrary");

        ff::MetalGraphicsDevice& graphicsDevice = static_cast<ff::MetalGraphicsDevice&>(ff::Locator::getGraphicsDevice());

        auto metalLibrary = std::make_shared<MetalLibrary>(assetBundle, assetObject);
        FF_ASSERT(metalLibrary, "Could not load MetalLibrary: `%s`.", assetObject["name"]); 

        _libraryMap.insert(std::make_pair(assetObject["name"], metalLibrary));

        for (const auto& shaderObject : assetObject["shaders"]) {
            FF_ASSERT(!shaderObject["name"].is_null(), "Missing key `name` in shader for library `%s`.", assetObject["name"]);
            FF_ASSERT(!shaderObject["vertex-function"].is_null(), "Missing key `vertex-function` in shader for library `%s`.", assetObject["name"]);
            FF_ASSERT(!shaderObject["fragment-function"].is_null(), "Missing key `fragment-function` in shader for library `%s`.", assetObject["name"]);

            std::unordered_set<int> vertexImageSlots;
            std::unordered_set<int> fragmentImageSlots;
            std::unordered_set<int> vertexUniformBufferSlots;
            std::unordered_set<int> fragmentUniformBufferSlots;
            for(auto bindingObject : shaderObject["bindings"]) {
                FF_ASSERT(!bindingObject["type"].is_null(), "Missing key `type` in binding for shader `%s`.", shaderObject["name"]);
                FF_ASSERT(!bindingObject["slot"].is_null(), "Missing key `slot` in binding for shader `%s`.", shaderObject["name"]);
                FF_ASSERT(!bindingObject["vertex-attachment"].is_null(), "Missing key `vertex-attachment` in binding for shader `%s`.", shaderObject["name"]);
                FF_ASSERT(!bindingObject["fragment-attachment"].is_null(), "Missing key `fragment-attachment` in binding for shader `%s`.", shaderObject["name"]);

                ShaderFunctionBindingType bindingType = convertStringToShaderFunctionBindingType(bindingObject["type"]);
                switch(bindingType) {
                case ShaderFunctionBindingType::Texture:
                    if(bindingObject["vertex-attachment"]) {
                        vertexImageSlots.insert(bindingObject["slot"].get<int>());
                    }
                    if(bindingObject["fragment-attachment"]) {
                        fragmentImageSlots.insert(bindingObject["slot"].get<int>());
                    }
                    break;
                case ShaderFunctionBindingType::UniformBuffer:
                    if(bindingObject["vertex-attachment"]) {
                        vertexUniformBufferSlots.insert(bindingObject["slot"].get<int>());
                    }
                    if(bindingObject["fragment-attachment"]) {
                        fragmentUniformBufferSlots.insert(bindingObject["slot"].get<int>());
                    }
                    break;
                }
            }

            _shaderMap.insert(std::make_pair(shaderObject["name"], std::make_shared<MetalShader>(graphicsDevice,
                metalLibrary->getFunction(shaderObject["vertex-function"]),
                metalLibrary->getFunction(shaderObject["fragment-function"]),
                vertexImageSlots,
                fragmentImageSlots,
                vertexUniformBufferSlots,
                fragmentUniformBufferSlots)));
        }
    }
    MetalShaderLibrary::~MetalShaderLibrary() {
    }

    std::shared_ptr<MetalShader> MetalShaderLibrary::getShader(const std::string& name) const {
        auto it = _shaderMap.find(name);
        FF_ASSERT(it != _shaderMap.end(), "Invalid shader name: `%s`.", name);
        return it->second;
    }
}
