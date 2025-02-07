/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_IRENDER_STATE_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_IRENDER_STATE_HPP

#include <ff/graphics/IShader.hpp>
#include <ff/graphics/IVertexDescriptor.hpp>
#include <ff/graphics/BufferFormats.hpp>
#include <ff/graphics/BlendState.hpp>

#include <optional>

#include <memory>
#include <vector>

namespace ff {
    struct RenderColorLayerState {
        RenderColorLayerState(ColorBufferFormat const& colorBufferFormat,
            std::optional<BlendState> const& blendState = std::nullopt)
            :colorBufferFormat(colorBufferFormat),
            blendState(blendState) {
        }

        ColorBufferFormat colorBufferFormat;
        std::optional<BlendState> blendState;
    };

    class IRenderState {
    public:
        IRenderState(std::shared_ptr<IShader> const& shader,
            std::shared_ptr<IVertexDescriptor> const& vertexDescriptor,
            std::vector<RenderColorLayerState> const& renderColorLayerStates,
            std::optional<DepthBufferFormat> const& depthBufferFormat);
        virtual ~IRenderState() = default;

        std::shared_ptr<IShader> getShader() const;
        std::shared_ptr<IVertexDescriptor> getVertexDescriptor() const;

        std::vector<RenderColorLayerState> const& getRenderColorLayerStates() const;

        std::optional<DepthBufferFormat> getDepthBufferFormat() const;

    private:
        std::shared_ptr<IShader> _shader;
        std::shared_ptr<IVertexDescriptor> _vertexDescriptor;

        std::vector<RenderColorLayerState> _renderColorLayerStates;
        std::optional<DepthBufferFormat> _depthBufferFormat;
    };

    class NullRenderState : public IRenderState {
    public:
        NullRenderState(std::shared_ptr<IShader> const& shader,
            std::shared_ptr<IVertexDescriptor> const& vertexDescriptor,
            std::vector<RenderColorLayerState> const& renderColorLayerStates,
            std::optional<DepthBufferFormat> const& depthBufferFormat)
            :IRenderState(shader,
                vertexDescriptor,
                renderColorLayerStates,
                depthBufferFormat) {
        }

        virtual ~NullRenderState() = default;

    private:
    };
}

#endif
