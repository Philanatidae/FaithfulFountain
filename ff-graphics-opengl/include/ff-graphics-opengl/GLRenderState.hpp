/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_RENDER_STATE_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_RENDER_STATE_HPP

#include <ff-graphics-opengl/gl.hpp>
#include <ff-graphics-opengl/GLGraphicsDevice.hpp>
#include <ff/graphics/IRenderState.hpp>

#include <ff-graphics-opengl/GLShader.hpp>
#include <ff-graphics-opengl/GLVertexDescriptor.hpp>
#include <ff-graphics-opengl/GLFramebuffer.hpp>

#include <memory>
#include <unordered_map>

namespace ff {
    class GLRenderState final : public IRenderState {
    public:
        GLRenderState(GLGraphicsDevice* const& graphicsDevice,
            std::shared_ptr<IShader> const& shader,
            std::shared_ptr<IVertexDescriptor> const& vertexDescriptor,
            std::vector<RenderColorLayerState> const& renderColorLayerStates,
            std::optional<DepthBufferFormat> const& depthBufferFormat);
        ~GLRenderState();

        void prepareForDrawCall(std::unordered_map<int, std::shared_ptr<IVertexBuffer>>& bufferSlots,
            std::unordered_map<int, int>& bufferOffsets,
            IIndexBuffer* const& indexBuffer);

    private:
        GLGraphicsDevice* const _graphicsDevice;

        std::unordered_map<uint64_t, GLuint> _vaos;
    };
}

#endif
