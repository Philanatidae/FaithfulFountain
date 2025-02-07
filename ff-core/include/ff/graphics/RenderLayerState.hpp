/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_RENDER_LAYER_STATE_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_RENDER_LAYER_STATE_HPP

#include <ff/graphics/BufferFormats.hpp>
#include <ff/graphics/BlendState.hpp>

#include <optional>

namespace ff {
    struct RenderLayerState {
        RenderLayerState(const ColorBufferFormat& colorBufferFormat,
            const DepthBufferFormat& depthBufferFormat = DepthBufferFormat::Invalid,
            const std::optional<BlendState>& blendState = std::nullopt)
            :colorBufferFormat(colorBufferFormat),
            depthBufferFormat(depthBufferFormat),
            blendState(blendState) {
        }

        ColorBufferFormat colorBufferFormat;
        DepthBufferFormat depthBufferFormat;
        std::optional<BlendState> blendState;
    };
}

#endif
