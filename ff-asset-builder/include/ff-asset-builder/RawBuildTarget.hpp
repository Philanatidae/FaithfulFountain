/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_FF_ASSET_BUILDER_RAW_BUILD_TARGET_HPP
#define _FAITHFUL_FOUNTAIN_FF_ASSET_BUILDER_RAW_BUILD_TARGET_HPP

#include <ff-asset-builder/BuildTarget.hpp>
#include <filesystem>

namespace ff {
    class RawBuildTarget : public BuildTarget {
    public:
        RawBuildTarget(const std::string& name);
        virtual ~RawBuildTarget();

        std::string getType() const override;
        std::string getName() const override;
        uint32_t getFlags() const override;

        std::vector<std::filesystem::path> getOutputs(AssetBuilder* builder) const override;

        void build(AssetBuilder* builder) override;
        void populateMetadata(nlohmann::json& targetObject) override;

    private:
        std::string _name;
    };
}

#endif
