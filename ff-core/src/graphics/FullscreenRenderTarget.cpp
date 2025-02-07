/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/FullscreenRenderTarget.hpp>

#include <ff/events/ResizeEvent.hpp>
#include <ff/graphics/BufferFormats.hpp>
#include <ff/messages/MessageBus.hpp>

#include <ff/Locator.hpp>

namespace ff {
    FullscreenRenderTarget::FullscreenRenderTarget(ColorBufferFormat const& colorBufferFormat,
        TextureUsage_t const& colorTargetUsage,
        std::string const& colorLabel)
        :FullscreenRenderTarget(colorBufferFormat,
            colorTargetUsage,
            colorLabel,
            DepthBufferFormat::Invalid,
            0,
            "") {
    }
    FullscreenRenderTarget::FullscreenRenderTarget(ColorBufferFormat const& colorBufferFormat,
        TextureUsage_t const& colorTargetUsage,
        std::string const& colorLabel,
        DepthBufferFormat const& depthBufferFormat,
        TextureUsage_t const& depthTargetUsage,
        std::string const& depthLabel)
        :_colorRenderTarget(colorLabel),
        _depthRenderTarget(depthLabel) {
        FF_ASSERT(Locator::getColorTextureManager().createReactiveTexture(colorLabel,
                colorBufferFormat,
                0,
                colorTargetUsage,
                1),
            "Error creating color render target.");
        if(depthBufferFormat != DepthBufferFormat::Invalid) {
            FF_ASSERT(Locator::getDepthTextureManager().createReactiveTexture(depthLabel,
                    depthBufferFormat,
                    0,
                    depthTargetUsage,
                    1),
                "Error creating color render target.");
        }
    }

    ResourceHandle<ColorTexture> FullscreenRenderTarget::getColorRenderTarget() const {
        return Locator::getColorTextureManager().getTexture(_colorRenderTarget);
    }
    ResourceHandle<DepthTexture> FullscreenRenderTarget::getDepthRenderTarget() const {
        return Locator::getDepthTextureManager().getTexture(_depthRenderTarget);
    }
}
