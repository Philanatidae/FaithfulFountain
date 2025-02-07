/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/Material.hpp>

#include <ff/Locator.hpp>

#include <tuple>

namespace ff {

Material::Material(std::string const& shader,
    Color const& tintColor,
    ResourceHandle<ColorTexture> diffuseTexture,
    std::shared_ptr<ISamplerState> const& diffuseSamplerState)
    :shader(shader),
    tintColor(tintColor),
    diffuseTexture(diffuseTexture),
    diffuseSamplerState(diffuseSamplerState) {
    update();
}
Material::Material(IAssetBundle& assetBundle, const nlohmann::json& assetObject) {
    FF_ASSET_TYPE_CHECK(assetObject, "Material");

    FF_ASSERT(!assetObject["shader"].is_null(), "Missing key `shader` in for asset `%s`.", assetObject["name"]);
    shader = assetObject["shader"];

    FF_ASSERT(!assetObject["tint-r"].is_null(), "Missing key `tint-r` in for asset `%s`.", assetObject["name"]);
    tintColor.r = assetObject["tint-r"];
    FF_ASSERT(!assetObject["tint-g"].is_null(), "Missing key `tint-g` in for asset `%s`.", assetObject["name"]);
    tintColor.g = assetObject["tint-g"];
    FF_ASSERT(!assetObject["tint-b"].is_null(), "Missing key `ting-b` in for asset `%s`.", assetObject["name"]);
    tintColor.b = assetObject["tint-b"];

    if(!assetObject["diffuse-texture"].is_null()) {
        diffuseTexture = assetBundle.load<ff::ColorTexture>(assetObject["diffuse-texture"]);
        // @todo Allow custom samplers. Maybe in Directory.xml?
        diffuseSamplerState = ff::Locator::getGraphicsDevice()
            .createSamplerState(ff::SamplerFilter::LINEAR, ff::SamplerFilter::LINEAR,
                ff::SamplerAddressMode::CLAMP_TO_EDGE, ff::SamplerAddressMode::CLAMP_TO_EDGE,
                ff::SamplerMipFilter::LINEAR);
    }

    update();
}
Material::Material(Material const& other)
    :shader(other.shader),
    tintColor(other.tintColor),
    diffuseTexture(other.diffuseTexture),
    diffuseSamplerState(other.diffuseSamplerState) {
    update();
}

void Material::update() {
    if(_materialUniforms == nullptr) {
        _materialUniforms = std::make_unique<MaterialUniforms>();
        _materialUniformsBuffer = Locator::getGraphicsDevice().createUniformBuffer<MaterialUniforms>();
    }

    _materialUniforms->color = tintColor;
    _materialUniformsBuffer->bufferData(_materialUniforms.get());
}
std::shared_ptr<UniformBuffer<MaterialUniforms>> Material::getUniformBuffer() const {
    return _materialUniformsBuffer;
}

bool Material::operator<(const Material& rhs) const {
    // Order of preference: shader, diffuseTexture, diffuseSamplerState
    return std::make_tuple(shader, diffuseTexture.get(), (ff::ISamplerState*)diffuseSamplerState.get())
        < std::make_tuple(rhs.shader, rhs.diffuseTexture.get(), (ff::ISamplerState*)diffuseSamplerState.get());
}

}
