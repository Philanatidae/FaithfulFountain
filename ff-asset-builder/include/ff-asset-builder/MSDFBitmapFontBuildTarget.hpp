/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ASSET_BUILDER_MSDF_BITMAP_FONT_BUILD_TARGET_HPP
#define _FAITHFUL_FOUNTAIN_ASSET_BUILDER_MSDF_BITMAP_FONT_BUILD_TARGET_HPP

#include <ff-asset-builder/BuildTarget.hpp>
#include <filesystem>
#include <pugixml.hpp>

namespace ff {
    class MSDFBitmapFontBuildTarget : public BuildTarget {
    public:
        MSDFBitmapFontBuildTarget(const std::string& name,
            const int& fontSize,
            const int& radius);
        virtual ~MSDFBitmapFontBuildTarget();

        std::string getType() const override;
        std::string getName() const override;
        uint32_t getFlags() const override;

        std::vector<std::filesystem::path> getOutputs(AssetBuilder* builder) const override;

        const int& getFontSize() const;
        const int& getRadius() const;

        void build(AssetBuilder* builder) override;
        void populateMetadata(nlohmann::json& targetObject) override;

    private:
        std::string _name;

        int _fontSize;
        int _radius;

        std::string _textureRegionName;
        pugi::xml_document _bmfontDoc;
    };
}

#endif
