/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ASSET_BUILDER_TEXTURE_ATLAS_BUILD_TARGET_HPP
#define _FAITHFUL_FOUNTAIN_ASSET_BUILDER_TEXTURE_ATLAS_BUILD_TARGET_HPP

#include <ff-asset-builder/BuildTarget.hpp>
#include <filesystem>

#include <vector>

namespace ff {
    class TextureAtlasBuildTarget : public BuildTarget {
    public:
        TextureAtlasBuildTarget(const std::string& name,
            const std::vector<std::string>& textureSourceNames,
            const int& borderSize,
            const int& padding,
            const bool& convertAllToPremultipliedAlpha);
        virtual ~TextureAtlasBuildTarget();

        std::string getType() const override;
        std::string getName() const override;
        uint32_t getFlags() const override;
        
        std::vector<std::filesystem::path> getOutputs(AssetBuilder* builder) const override;

        const std::vector<std::string>& getTextureSourceNames() const;
        const int& getBorderSize() const;
        const int& getPadding() const;

        void build(AssetBuilder* builder) override;
        void populateMetadata(nlohmann::json& targetObject) override;

    private:
        std::string _name;
        std::vector<std::string> _textureSourceNames;
        int _borderSize;
        int _padding;
        bool _convertAllToPremultipliedAlpha;
        bool _isAlphaPremultiplied;
    };
}

#endif
