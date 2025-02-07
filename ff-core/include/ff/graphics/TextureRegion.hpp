/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_TEXTURE_REGION_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_TEXTURE_REGION_HPP

#include <ff/graphics/Texture.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <ff/assets/IAssetBundle.hpp>

namespace ff {
    class TextureRegion {
    public:
        TextureRegion(const ResourceHandle<ColorTexture>& texture,
            const int& sx, const int& sy,
            const int& sw, const int& sh);
        TextureRegion(const ResourceHandle<TextureRegion>& texture,
            const int& sx, const int& sy,
            const int& sw, const int& sh);
        TextureRegion(const TextureRegion& texture,
            const int& sx, const int& sy,
            const int& sw, const int& sh);
        TextureRegion(IAssetBundle& assetBundle, const nlohmann::json& assetObject);
        virtual ~TextureRegion();

        const ResourceHandle<ColorTexture> getTexture() const;

        const int& getSourceX() const;
        const int& getSourceY() const;
        const int& getSourceWidth() const;
        const int& getSourceHeight() const;

        const glm::vec2& getTexelMin() const;
        const glm::vec2& getTexelMax() const;

    private:
        ResourceHandle<ColorTexture> _texture;

        int _sx;
        int _sy;
        int _sw;
        int _sh;

        glm::vec2 _texelMin;
        glm::vec2 _texelMax;

        void computeTexelCoords();
    };
}

#endif
