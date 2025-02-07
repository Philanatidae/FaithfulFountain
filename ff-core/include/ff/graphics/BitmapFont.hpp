/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_BITMAP_FONT_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_BITMAP_FONT_HPP

#include <ff/graphics/TextureRegion.hpp>
#include <ff/assets/IAssetBundle.hpp>
#include <unordered_map>
#include <map>
#include <glm/glm.hpp>
#include <utility>

namespace ff {
    struct Glyph {
        Glyph(float const& advance, glm::vec2 const& offset, std::shared_ptr<TextureRegion> const& region)
            :advance(advance),offset(offset),region(region) {
        }

        float advance;
        glm::vec2 offset;
        std::shared_ptr<TextureRegion> region;
    };
    struct Kerning {
        Kerning(float const& amount)
            :amount(amount) {
        }

        float amount;
    };
    enum class Justify {
        CENTER,
        LEFT,
        RIGHT
    };

    typedef std::unordered_map<char, Glyph> GlyphMap;
    typedef std::pair<char, char> CharPair;
    typedef std::map<CharPair, Kerning> KerningMap;

    class BitmapFont {
    public:
        BitmapFont(const GlyphMap& glyphMap, const float& lineHeight, const float& base, const bool& msdf = false, const KerningMap& kerningMap = {});
        BitmapFont(IAssetBundle& assetBundle, const nlohmann::json& assetObject);
        virtual ~BitmapFont();

        bool containsGlyph(const char& c) const;
        const Glyph& getGlyph(const char& c) const;

        bool containsKerning(const char& a, const char& b) const;
        const Kerning& getKerning(const char& a, const char& b) const;

        const float& getLineHeight() const;
        const float& getBase() const;

        const bool& renderAsMSDF() const;

        glm::vec2 measure(const std::string& content) const;

    private:
        GlyphMap _glyphMap;
        KerningMap _kerningMap;
        float _lineHeight;
        float _base;
        bool _msdf;
    };
}

#endif
