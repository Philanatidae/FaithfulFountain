/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/BitmapFont.hpp>

#include <ff/Locator.hpp>

#include <glm/glm.hpp>
#include <sstream>
#include <cstdio>
#include <cstring>

namespace ff {
    BitmapFont::BitmapFont(const GlyphMap& glyphMap, const float& lineHeight, const float& base, const bool& msdf, const KerningMap& kerningMap)
        :_glyphMap(glyphMap),_kerningMap(kerningMap),_lineHeight(lineHeight),_base(base),_msdf(msdf) {
    }
    BitmapFont::BitmapFont(IAssetBundle& assetBundle, const nlohmann::json& assetObject) {
        FF_ASSET_TYPE_CHECK(assetObject, "BitmapFont");

        FF_ASSERT(!assetObject["line-height"].is_null(), "Missing key `line-height` in bitmap font for asset `%s`.", assetObject["name"]);
        _lineHeight = assetObject["line-height"];
        FF_ASSERT(!assetObject["base"].is_null(), "Missing key `base` in bitmap font for asset `%s`.", assetObject["name"]);
        _base = assetObject["base"];

        // @todo Support multiple pages
        FF_ASSERT(!assetObject["pages"].is_null(), "Missing key `pages` in bitmap font for asset `%s`.", assetObject["name"]);
        FF_ASSERT(assetObject["pages"].size() == 1, "Bitmap fonts with multiple pages are not supported for asset `%s`.", assetObject["name"]);
        ResourceHandle<TextureRegion> fontAtlasTexture = assetBundle.load<TextureRegion>(assetObject["pages"]["0"]);

        FF_ASSERT(!assetObject["distance-field-type"].is_null(), "Missing key `distance-field-type` in bitmap font for asset `%s`.", assetObject["name"]);
        FF_ASSERT(assetObject["distance-field-type"] == "msdf", "Only MSDF distance field type is supported for asset `%s`.", assetObject["name"]);
        // @todo Add support for none (plain bitmap) and signed distance fields.
        _msdf = true;

        auto paddingObject = assetObject["padding"];
        FF_ASSERT(!paddingObject.is_null(), "Missing key `padding` in bitmap font for asset `%s`.", assetObject["name"]);
        FF_ASSERT(paddingObject.size() == 4, "Invalid padding in bitmap font for asset `%s`.", assetObject["name"]);
        FF_ASSERT(!paddingObject["top"].is_null(), "Missing key `top` in bitmap font for asset `%s`.", assetObject["name"]);
        float paddingTop = paddingObject["top"];
        FF_ASSERT(!paddingObject["right"].is_null(), "Missing key `right` in bitmap font for asset `%s`.", assetObject["name"]);
        float paddingRight = paddingObject["right"];
        FF_ASSERT(!paddingObject["bottom"].is_null(), "Missing key `bottom` in bitmap font for asset `%s`.", assetObject["name"]);
        float paddingBottom = paddingObject["bottom"];
        FF_ASSERT(!paddingObject["left"].is_null(), "Missing key `left` in bitmap font for asset `%s`.", assetObject["name"]);
        float paddingLeft = paddingObject["left"];
        // @todo Padding doesn't actually do anything, but needs to be supported. Alternatively, we switch to texture regions for glyphs and padding is already handled.

        auto charsArray = assetObject["chars"];
        FF_ASSERT(charsArray.is_array(), "Missing key `chars` in bitmap font for asset `%s`.", assetObject["name"]);
        for(auto it = charsArray.begin(); it != charsArray.end(); ++it) {
            auto charObject = *it;
            FF_ASSERT(charObject.is_object(), "Invalid character in bitmap font for asset `%s`.", assetObject["name"]);
            FF_ASSERT(!charObject["id"].is_null(), "Missing key `id` in bitmap font for asset `%s`.", assetObject["name"]);
            char c = (char)charObject["id"].get<int>();

            FF_ASSERT(!charObject["x"].is_null(), "Missing key `x` in bitmap font for asset `%s`.", assetObject["name"]);
            float x = charObject["x"];
            FF_ASSERT(!charObject["y"].is_null(), "Missing key `y` in bitmap font for asset `%s`.", assetObject["name"]);
            float y = charObject["y"];
            FF_ASSERT(!charObject["width"].is_null(), "Missing key `width` in bitmap font for asset `%s`.", assetObject["name"]);
            float width = charObject["width"];
            FF_ASSERT(!charObject["height"].is_null(), "Missing key `height` in bitmap font for asset `%s`.", assetObject["name"]);
            float height = charObject["height"];

            FF_ASSERT(!charObject["x-offset"].is_null(), "Missing key `x-offset` in bitmap font for asset `%s`.", assetObject["name"]);
            float xoffset = charObject["x-offset"];
            FF_ASSERT(!charObject["y-offset"].is_null(), "Missing key `y-offset` in bitmap font for asset `%s`.", assetObject["name"]);
            float yoffset = charObject["y-offset"];
            FF_ASSERT(!charObject["x-advance"].is_null(), "Missing key `x-advance` in bitmap font for asset `%s`.", assetObject["name"]);
            float xadvance = charObject["x-advance"];

            std::shared_ptr<TextureRegion> region = std::make_shared<TextureRegion>(fontAtlasTexture,
                x, y,
                width, height);

            _glyphMap.emplace(c, Glyph(xadvance, 
                glm::vec2(xoffset, -yoffset), // Flip y because FF coordinate y+ = up.
                region));
        }

        auto kerningsArray = assetObject["kernings"];
        FF_ASSERT(kerningsArray.is_array(), "Missing key `kernings` in bitmap font for asset `%s`.", assetObject["name"]);
        for(auto it = kerningsArray.begin(); it != kerningsArray.end(); ++it) {
            auto kerningObject = *it;
            FF_ASSERT(kerningObject.is_object(), "Invalid kerning in bitmap font for asset `%s`.", assetObject["name"]);
            FF_ASSERT(!kerningObject["first"].is_null(), "Missing key `first` in bitmap font for asset `%s`.", assetObject["name"]);
            char first = (char)kerningObject["first"].get<int>();
            FF_ASSERT(!kerningObject["second"].is_null(), "Missing key `second` in bitmap font for asset `%s`.", assetObject["name"]);
            char second = (char)kerningObject["second"].get<int>();
            FF_ASSERT(!kerningObject["amount"].is_null(), "Missing key `amount` in bitmap font for asset `%s`.", assetObject["name"]);
            float amount = kerningObject["amount"];

            _kerningMap.emplace(std::make_pair(first, second), Kerning(amount));
        }
    }
    BitmapFont::~BitmapFont() {
    }

    bool BitmapFont::containsGlyph(const char& c) const {
        return _glyphMap.find(c) != _glyphMap.end();
    }
    const Glyph& BitmapFont::getGlyph(const char& c) const {
        return _glyphMap.find(c)->second;
    }

    bool BitmapFont::containsKerning(const char& a, const char& b) const {
        return _kerningMap.find(std::make_pair(a, b)) != _kerningMap.end();
    }
    const Kerning& BitmapFont::getKerning(const char& a, const char& b) const {
        return _kerningMap.find(std::make_pair(a, b))->second;
    }

    const float& BitmapFont::getLineHeight() const {
        return _lineHeight;
    }
    const float& BitmapFont::getBase() const {
        return _base;
    }

    const bool& BitmapFont::renderAsMSDF() const {
        return _msdf;
    }

    glm::vec2 BitmapFont::measure(const std::string& content) const {
        // @todo Kerning
        // @todo Newline
        // @todo Tabs
        // @todo I don't believe this algorithm is actually correct

        glm::vec2 bounds(0, getBase());
        glm::vec2 pen(0);

        bool eligibleForKerning = false;
        for(size_t i = 0;
            i < content.size();
            i++) {
            switch(content[i]) {
                case '\t':
                    FF_CONSOLE_WARN("Tabs not implemented.");
                case '\r':
                    continue;
                case '\n':
                    eligibleForKerning = false;
                    pen.x = 0;
                    pen.y -= getLineHeight();
                    bounds.y += getLineHeight();
                    continue;
                default:
                    break;
            }

            auto& glyph = getGlyph(content[i]);
            if(i != 0) {
                char a = content[i - 1],
                    b = content[i];
                if(eligibleForKerning && containsKerning(a, b)) {
                    pen.x += getKerning(a, b).amount;
                }
            }
            if(i < content.size() - 1 || true) {
                pen.x += glyph.advance;
            } else {
                pen.x += glyph.region->getSourceWidth();
            }

            bounds.x = glm::max(bounds.x, pen.x);

            eligibleForKerning = true;
        }

        return bounds;
    }
}
