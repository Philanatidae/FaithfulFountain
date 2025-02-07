/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-asset-builder/TextureRegionBuildTarget.hpp>

namespace ff {
    TextureRegionBuildTarget::TextureRegionBuildTarget(const std::string& name,
        const std::string& textureName,
        const int& sx,
        const int& sy,
        const int& sw,
        const int& sh)
        :_name(name),_textureName(textureName),_sx(sx),_sy(sy),_sw(sw),_sh(sh) {
    }
    TextureRegionBuildTarget::~TextureRegionBuildTarget() {
    }

    std::string TextureRegionBuildTarget::getType() const {
        return "TextureRegion";
    }
    std::string TextureRegionBuildTarget::getName() const {
        return _name;
    }
    uint32_t TextureRegionBuildTarget::getFlags() const {
        return 0;
    }

    std::vector<std::filesystem::path> TextureRegionBuildTarget::getOutputs(AssetBuilder* builder) const {
        // No outputs; purely metadata
        return {};
    }

    const int& TextureRegionBuildTarget::getSourceX() const {
        return _sx;
    }
    const int& TextureRegionBuildTarget::getSourceY() const {
        return _sy;
    }
    const int& TextureRegionBuildTarget::getSourceWidth() const {
        return _sw;
    }
    const int& TextureRegionBuildTarget::getSourceHeight() const {
        return _sh;
    }

    void TextureRegionBuildTarget::build(AssetBuilder* builder) {
    }
    void TextureRegionBuildTarget::populateMetadata(nlohmann::json& targetObject) {
        targetObject["texture"] = _textureName;
        targetObject["sx"] = _sx;
        targetObject["sy"] = _sy;
        targetObject["sw"] = _sw;
        targetObject["sh"] = _sh;
    }
}
