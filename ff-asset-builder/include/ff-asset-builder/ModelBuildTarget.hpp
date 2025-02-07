/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ASSET_BUILDER_MODEL_BUILD_TARGET_HPP
#define _FAITHFUL_FOUNTAIN_ASSET_BUILDER_MODEL_BUILD_TARGET_HPP

#include <ff-asset-builder/BuildTarget.hpp>

#include <vector>
#include <utility>

namespace ff {

class ModelBuildTarget : public BuildTarget {
public:
    ModelBuildTarget(std::string const& name,
        std::vector<std::string> const& validates,
        std::vector<std::pair<std::string, std::string>> const& materialAssignments);
    virtual ~ModelBuildTarget() = default;

    std::string getType() const override;
    std::string getName() const override;
    uint32_t getFlags() const override;

    std::vector<std::filesystem::path> getOutputs(AssetBuilder* builder) const override;

    void build(AssetBuilder* builder) override;
    void populateMetadata(nlohmann::json& targetObject) override;

private:
    std::string _name;
    std::vector<std::string> _validates;
    std::vector<std::pair<std::string, std::string>> _materialAssignments;
};

}

#endif
