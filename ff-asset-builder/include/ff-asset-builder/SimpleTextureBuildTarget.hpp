/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_FF_ASSET_BUILDER_SIMPLE_TEXTURE_BUILD_TARGET_HPP
#define _FAITHFUL_FOUNTAIN_FF_ASSET_BUILDER_SIMPLE_TEXTURE_BUILD_TARGET_HPP

#include <ff-asset-builder/BuildTarget.hpp>
#include <filesystem>

namespace ff {
    class SimpleTextureBuildTarget : public BuildTarget {
    public:
        SimpleTextureBuildTarget(const std::string& name,
            const std::string& textureRegionName,
            const bool& isAlphaPremultiplied,
            const bool& mipMap);
        virtual ~SimpleTextureBuildTarget();

        std::string getType() const override;
        std::string getName() const override;
        uint32_t getFlags() const override;

        std::vector<std::filesystem::path> getOutputs(AssetBuilder* builder) const override;

        const bool& isAlphaPremultiplied() const;

        const std::string& getTextureRegionName() const;

        void build(AssetBuilder* builder) override;
        void populateMetadata(nlohmann::json& targetObject) override;

    private:
        std::string _textureRegionName;
        std::string _name;
        bool _isAlphaPremultiplied;
        bool _mipMap;
    };
}

#endif
