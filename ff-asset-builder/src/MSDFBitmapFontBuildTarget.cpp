/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-asset-builder/MSDFBitmapFontBuildTarget.hpp>

#include <ff-asset-builder/AssetBuilder.hpp>

#include <tinyformat/tinyformat.h>
#include <ff/Console.hpp>
#include <ff/util/OS.hpp>

#include <ff-asset-builder/SimpleTextureBuildTarget.hpp>

namespace ff {
    MSDFBitmapFontBuildTarget::MSDFBitmapFontBuildTarget(const std::string& name,
        const int& fontSize,
        const int& radius)
        :_name(name),
        _fontSize(fontSize),_radius(radius) {
    }
    MSDFBitmapFontBuildTarget::~MSDFBitmapFontBuildTarget() {
    }

    std::string MSDFBitmapFontBuildTarget::getType() const {
        return "BitmapFont";
    }
    std::string MSDFBitmapFontBuildTarget::getName() const {
        return _name;
    }
    uint32_t MSDFBitmapFontBuildTarget::getFlags() const {
        return 0;
    }

    std::vector<std::filesystem::path> MSDFBitmapFontBuildTarget::getOutputs(AssetBuilder* builder) const {
        return {
            builder->getTargetDir()/tinyformat::format("texture_%s", _name)
        };
    }

    const int& MSDFBitmapFontBuildTarget::getFontSize() const {
        return _fontSize;
    }
    const int& MSDFBitmapFontBuildTarget::getRadius() const {
        return _radius;
    }

    void MSDFBitmapFontBuildTarget::build(AssetBuilder* builder) {
        FF_ASSERT(getInputs().size() == 1, "MSDFBitmapFontBuildTarget must have exactly one input.");

        // @todo Move away from msdf-bmfont-builder and directly generate within ff-asset-builder.
        std::string msdfBMFontCmd = tinyformat::format("msdf-bmfont --smart-size --pot -r %s --font-size %s -b 1 -o %s %s",
            getRadius(),
            getFontSize(),
            (builder->getObjectDir()/getName()).string(),
            getInputs()[0].string());
        FF_ASSERT(builder->runCommand(msdfBMFontCmd), "Error while generating MSDF.");

        std::string fontFileName = tinyformat::format("%s.fnt",
            removeFileExtension(getInputs()[0].filename().string()));
        std::filesystem::path fontFilePath = builder->getObjectDir()/fontFileName;
        FF_ASSERT(std::filesystem::exists(fontFilePath), "Expected `%s` to generate `.fnt` file at `%s`.", getName(), fontFilePath);
        std::filesystem::path atlasFilePath = builder->getObjectDir()/std::filesystem::path(getName() + ".png");
        FF_ASSERT(std::filesystem::exists(atlasFilePath), "Expected `%s` to generate font atlas at `%s`.", getName(), atlasFilePath);

        _textureRegionName = tinyformat::format("texture_%s", getName());
        auto target = builder->addBuildTarget(_textureRegionName,
            std::make_shared<SimpleTextureBuildTarget>(_textureRegionName,
                _textureRegionName,
                false,
                false));
        target->addInput(atlasFilePath);
        builder->addTargetToBuild(_textureRegionName);
        addProduct(target->getName());

        auto loadResult = _bmfontDoc.load_file(fontFilePath.c_str());
        FF_ASSERT(loadResult, "Error while loading `.fnt` file at `%s` for `%s` (%s).", fontFilePath, getName(), loadResult.description());
    }
    void MSDFBitmapFontBuildTarget::populateMetadata(nlohmann::json& targetObject) {
        pugi::xml_node fontNode = _bmfontDoc.child("font");

        targetObject["distance-field-type"] = "msdf";
        targetObject["distance-range"] = fontNode.child("distanceField").attribute("distanceRange").as_float();

        targetObject["size"] = fontNode.child("info").attribute("size").as_int();
        targetObject["stretch-h"] = fontNode.child("info").attribute("stretchH").as_int();
        targetObject["padding"] = nlohmann::json::object();
        float paddingUp = 0, paddingRight = 0, paddingDown = 0, paddingLeft = 0;
        std::sscanf(fontNode.child("info").attribute("padding").as_string(), "%f,%f,%f,%f", &paddingUp, &paddingRight, &paddingDown, &paddingLeft);
        targetObject["padding"]["top"] = paddingUp;
        targetObject["padding"]["right"] = paddingRight;
        targetObject["padding"]["bottom"] = paddingDown;
        targetObject["padding"]["left"] = paddingLeft;
        targetObject["spacing"] = fontNode.child("info").attribute("spacing").as_int();

        targetObject["line-height"] = fontNode.child("common").attribute("lineHeight").as_int();
        targetObject["base"] = fontNode.child("common").attribute("base").as_int();
        targetObject["scale-w"] = fontNode.child("common").attribute("scaleW").as_int();
        targetObject["scale-h"] = fontNode.child("common").attribute("scaleH").as_int();
        targetObject["pages"] = fontNode.child("common").attribute("pages").as_int();
        targetObject["packed"] = fontNode.child("common").attribute("packed").as_int() == 1;
        targetObject["alpha-channel"] = fontNode.child("common").attribute("alphaChnl").as_int();
        targetObject["red-channel"] = fontNode.child("common").attribute("redChnl").as_int();
        targetObject["green-channel"] = fontNode.child("common").attribute("greenChnl").as_int();
        targetObject["blue-channel"] = fontNode.child("common").attribute("blueChnl").as_int();

        targetObject["pages"] = nlohmann::json::object();
        targetObject["pages"]["0"] = _textureRegionName;

        targetObject["chars"] = nlohmann::json::array();
        for (pugi::xml_node charNode : fontNode.child("chars").children("char")) {
            nlohmann::json charObject;
            charObject["id"] = charNode.attribute("id").as_int();
            charObject["x"] = charNode.attribute("x").as_int();
            charObject["y"] = charNode.attribute("y").as_int();
            charObject["width"] = charNode.attribute("width").as_int();
            charObject["height"] = charNode.attribute("height").as_int();
            charObject["x-offset"] = charNode.attribute("xoffset").as_int();
            charObject["y-offset"] = charNode.attribute("yoffset").as_int();
            charObject["x-advance"] = charNode.attribute("xadvance").as_int();
            charObject["page"] = charNode.attribute("page").as_int();

            targetObject["chars"].push_back(charObject);
        }

        targetObject["kernings"] = nlohmann::json::array();
        for (pugi::xml_node kerningNode : fontNode.child("kernings").children("kerning")) {
            nlohmann::json kerningObject;
            kerningObject["first"] = kerningNode.attribute("first").as_int();
            kerningObject["second"] = kerningNode.attribute("second").as_int();
            kerningObject["amount"] = kerningNode.attribute("amount").as_int();

            targetObject["kernings"].push_back(kerningObject);
        }
    }
}
