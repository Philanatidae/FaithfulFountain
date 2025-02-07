/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/TextureRegion.hpp>

#include <ff/graphics/TextureData.hpp>
#include <ff/Locator.hpp>

namespace ff {
    TextureRegion::TextureRegion(const ResourceHandle<ColorTexture>& texture,
        const int& sx, const int& sy,
        const int& sw, const int& sh)
        :_texture(texture),_sx(sx),_sy(sy),_sw(sw),_sh(sh) {
        computeTexelCoords();
    }
    TextureRegion::TextureRegion(const ResourceHandle<TextureRegion>& texture,
        const int& sx, const int& sy,
        const int& sw, const int& sh)
        :TextureRegion(*texture, sx, sy, sw, sh) {
    }
    TextureRegion::TextureRegion(const TextureRegion& texture,
        const int& sx, const int& sy,
        const int& sw, const int& sh)
        :TextureRegion(texture._texture,
            texture._sx + sx,
            texture._sy + sy,
            sw,
            sh) {
    }
    TextureRegion::TextureRegion(IAssetBundle& assetBundle, const nlohmann::json& assetObject) {
        if(assetObject["type"] == "TextureRegion") {
            FF_ASSERT(!assetObject["texture"].is_null(), "Missing key `texture` in asset `%s`.", assetObject["name"]);

            _texture = assetBundle.load<ColorTexture>(assetObject["texture"]);

            FF_ASSERT(assetObject["sx"].is_number(), "Missing key `sx` in texture region for asset `%s`.", assetObject["name"]);
            FF_ASSERT(assetObject["sy"].is_number(), "Missing key `sy` in texture region for asset `%s`.", assetObject["name"]);
            FF_ASSERT(assetObject["sw"].is_number(), "Missing key `sw` in texture region for asset `%s`.", assetObject["name"]);
            FF_ASSERT(assetObject["sh"].is_number(), "Missing key `sh` in texture region for asset `%s`.", assetObject["name"]);

            _sx = assetObject["sx"];
            _sy = assetObject["sy"];
            _sw = assetObject["sw"];
            _sh = assetObject["sh"];
        } else if(assetObject["type"] == "Texture") {
            _texture = assetBundle.load<ColorTexture>(assetObject["name"]);

            _sx = _sy = 0;
            _sw = _texture->getWidth();
            _sh = _texture->getHeight();
        } else {
            FF_CONSOLE_FATAL("Incorrect type. Expected Texture or TextureRegion, type is `%s`.", assetObject["type"]);
        }

        computeTexelCoords();
    }
    TextureRegion::~TextureRegion() {
    }

    const ResourceHandle<ColorTexture> TextureRegion::getTexture() const {
        return _texture;
    }

    const int& TextureRegion::getSourceX() const {
        return _sx;
    }
    const int& TextureRegion::getSourceY() const {
        return _sy;
    }
    const int& TextureRegion::getSourceWidth() const {
        return _sw;
    }
    const int& TextureRegion::getSourceHeight() const {
        return _sh;
    }

    const glm::vec2& TextureRegion::getTexelMin() const {
        return _texelMin;
    }
    const glm::vec2& TextureRegion::getTexelMax() const {
        return _texelMax;
    }

    void TextureRegion::computeTexelCoords() {
        _texelMin = glm::vec2(((float)_sx / _texture->getWidth()),
            ((float)_sy / _texture->getHeight()));
        _texelMax = glm::vec2(((float)(_sx + _sw) / _texture->getWidth()),
            ((float)(_sy + _sh) / _texture->getHeight()));   
    }
}
