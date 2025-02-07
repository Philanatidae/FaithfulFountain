/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/IRenderState.hpp>

#include <ff/Console.hpp>

namespace ff {
    IRenderState::IRenderState(std::shared_ptr<IShader> const& shader,
        std::shared_ptr<IVertexDescriptor> const& vertexDescriptor,
        std::vector<RenderColorLayerState> const& renderColorLayerStates,
        std::optional<DepthBufferFormat> const& depthBufferFormat)
        :_shader(shader),
        _vertexDescriptor(vertexDescriptor),
        _renderColorLayerStates(renderColorLayerStates),
        _depthBufferFormat(depthBufferFormat) {
        // Make sure that all layers have the same
        // blend state. This can be removed if targeting GLES 3.2.
        // Or, it could be added as an "override", so that the
        // game can use independent blend states if the graphics
        // driver supports it, but have an alternative for GLES 3.0.
        //
        // This can be "worked around" by rendering once per blend state,
        // but given that this is GLES (i.e. mobile devices), the engine
        // won't do this automatically because of the huge performance hit.
        // The game can make its own decision on this regard.
        // @todo Add override.
        // @todo Check for minimum GLES version.
        for(int i = 1; i < renderColorLayerStates.size(); i++) {
            FF_ASSERT(renderColorLayerStates[i].blendState
                == renderColorLayerStates[i].blendState,
                "Programmable blend states disabled for GLES 3.0 support (first supported in 3.2). All blend states must be identical.");
        }
    }

    std::shared_ptr<IShader> IRenderState::getShader() const {
        return _shader;
    }
    std::shared_ptr<IVertexDescriptor> IRenderState::getVertexDescriptor() const {
        return _vertexDescriptor;
    }

    std::vector<RenderColorLayerState> const& IRenderState::getRenderColorLayerStates() const {
        return _renderColorLayerStates;
    }

    std::optional<DepthBufferFormat> IRenderState::getDepthBufferFormat() const {
        return _depthBufferFormat;
    }
         
}
