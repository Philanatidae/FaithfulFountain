/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_FULLSCREEN_RENDER_TARGET_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_FULLSCREEN_RENDER_TARGET_HPP

#include <ff/graphics/Texture.hpp>
#include <ff/graphics/BufferFormats.hpp>

namespace ff {
    class FullscreenRenderTarget final {
    public:
        FullscreenRenderTarget(ColorBufferFormat const& colorBufferFormat,
            TextureUsage_t const& colorTargetUsage,
            std::string const& colorLabel);
        FullscreenRenderTarget(ColorBufferFormat const& colorBufferFormat,
            TextureUsage_t const& colorTargetUsage,
            std::string const& colorLabel,
            DepthBufferFormat const& depthBufferFormat,
            TextureUsage_t const& depthTargetUsage,
            std::string const& depthLabel);

        ResourceHandle<ColorTexture> getColorRenderTarget() const;
        ResourceHandle<DepthTexture> getDepthRenderTarget() const;

    private:
        std::string _colorRenderTarget;
        std::string _depthRenderTarget;
    };
}

#endif
