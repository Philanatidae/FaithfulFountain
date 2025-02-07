/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-asset-builder/MaterialBuildTarget.hpp>

namespace ff {

MaterialBuildTarget::MaterialBuildTarget(const std::string& name,
    const std::string& shaderName,
    const int& tintR, const int& tintG, const int& tintB,
    const std::optional<std::string>& diffuseTextureName)
    :_name(name),_shaderName(shaderName),
    _tintR(tintR),_tintG(tintG),_tintB(tintB),
    _diffuseTextureName(diffuseTextureName) {
}

std::string MaterialBuildTarget::getType() const {
    return "Material";
}
std::string MaterialBuildTarget::getName() const {
    return _name;
}
uint32_t MaterialBuildTarget::getFlags() const {
    return 0;
}

std::vector<std::filesystem::path> MaterialBuildTarget::getOutputs(AssetBuilder* builder) const {
    // No outputs, metadata only
    return {};
}

void MaterialBuildTarget::build(AssetBuilder* builder) {
}
void MaterialBuildTarget::populateMetadata(nlohmann::json& targetObject) {
    targetObject["shader"] = _shaderName;
    targetObject["tint-r"] = (float)_tintR/255.0f;
    targetObject["tint-g"] = (float)_tintG/255.0f;
    targetObject["tint-b"] = (float)_tintB/255.0f;
    if(_diffuseTextureName) {
        targetObject["diffuse-texture"] = *_diffuseTextureName;
    }
}

}
