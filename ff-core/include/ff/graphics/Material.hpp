/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_MATERIAL_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_MATERIAL_HPP

#include <string>
#include <memory>
#include <ff/graphics/Color.hpp>
#include <ff/graphics/Texture.hpp>
#include <ff/graphics/ISamplerState.hpp>
#include <ff/graphics/UniformBuffer.hpp>
#include <ff/assets/IAssetBundle.hpp>

namespace ff {

struct MaterialUniforms {
    ff::Color color;
};

class Material {
public:
    Material(std::string const& shader,
        Color const& tintColor,
        ResourceHandle<ColorTexture> diffuseTexture,
        std::shared_ptr<ISamplerState> const& diffuseSamplerState);
    Material(IAssetBundle& assetBundle, const nlohmann::json& assetObject);
    Material(Material const& other);

    void update();
    std::shared_ptr<UniformBuffer<MaterialUniforms>> getUniformBuffer() const;

    bool operator<(const Material& rhs) const;

    std::string shader;
    Color tintColor;
    ResourceHandle<ColorTexture> diffuseTexture;
    std::shared_ptr<ISamplerState> diffuseSamplerState;
     
private:
    std::unique_ptr<MaterialUniforms> _materialUniforms;
    std::shared_ptr<UniformBuffer<MaterialUniforms>> _materialUniformsBuffer;
};

}

#endif
