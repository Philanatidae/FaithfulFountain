/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-asset-builder/RawBuildTarget.hpp>

#include <ff-asset-builder/AssetBuilder.hpp>

#include <ff/Console.hpp>
#include <ff/util/OS.hpp>

namespace ff {
    RawBuildTarget::RawBuildTarget(const std::string& name)
        :_name(name) {
    }
    RawBuildTarget::~RawBuildTarget() {
    }

    std::string RawBuildTarget::getType() const {
        return "Raw";
    }
    std::string RawBuildTarget::getName() const {
        return _name;
    }
    uint32_t RawBuildTarget::getFlags() const {
        return 0;
    }

    std::vector<std::filesystem::path> RawBuildTarget::getOutputs(AssetBuilder* builder) const {
        return {
            builder->getTargetDir()/_name
        };
    }

    void RawBuildTarget::build(AssetBuilder* builder) {
        FF_ASSERT(getInputs().size() == 1, "RawBuildTarget must have exactly one input.");
        auto& sourcePath = getInputs()[0];
        copyFileIfNewer(sourcePath, getOutputs(builder)[0]);
    }
    void RawBuildTarget::populateMetadata(nlohmann::json& targetObject) {
        targetObject["path"] = _name;
    }
}
