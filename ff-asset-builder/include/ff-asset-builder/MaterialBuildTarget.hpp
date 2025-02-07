/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ASSET_BUILDER_MATERIAL_BUILD_TARGET_HPP
#define _FAITHFUL_FOUNTAIN_ASSET_BUILDER_MATERIAL_BUILD_TARGET_HPP

#include <ff-asset-builder/BuildTarget.hpp>
#include <string>
#include <optional>
#include <filesystem>

namespace ff {

class MaterialBuildTarget : public BuildTarget {
public:
    MaterialBuildTarget(const std::string& name,
        const std::string& shaderName,
        const int& tintR, const int& tintG, const int& tintB,
        const std::optional<std::string>& diffuseTextureName);
    virtual ~MaterialBuildTarget() = default;

    std::string getType() const override;
    std::string getName() const override;
    uint32_t getFlags() const override;

    std::vector<std::filesystem::path> getOutputs(AssetBuilder* builder) const override;

    void build(AssetBuilder* builder) override;
    void populateMetadata(nlohmann::json& targetObject) override;

private:
    std::string _name;
    std::string _shaderName;
    int _tintR, _tintG, _tintB;
    std::optional<std::string> _diffuseTextureName;
};

}

#endif
