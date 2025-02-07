/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_RENDER_PASS_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_RENDER_PASS_HPP

#include <ff-graphics-opengl/gl.hpp>
#include <ff/graphics/IRenderPass.hpp>

#include <ff-graphics-opengl/GLGraphicsDevice.hpp>
#include <ff-graphics-opengl/GLFramebuffer.hpp>

#include <unordered_map>

namespace ff {
    class GLRenderState;
    class GLSamplerState;
    class GLFramebuffer;

    class GLRenderPass : public IRenderPass {
    public:
        GLRenderPass(GLGraphicsDevice* const& graphicsDevice,
            std::vector<std::pair<ResourceHandle<ColorTexture>,
                RenderTargetColorAttachmentOptions>> const& colorTargets,
            std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> const& depthTarget);
        ~GLRenderPass() = default;

    protected:
        void setStateImpl(const std::shared_ptr<IRenderState>& renderState) override;

        void setCullModeImpl(const CullMode& cullMode) override;

        void setViewportImpl(int const& x, int const& y,
            int const& width, int const& height) override;
        void setScissorRectImpl(int const& x, int const& y,
            int const& width, int const& height) override;

        void setTextureImpl(const ResourceHandle<ColorTexture> &texture,
            const int &slot = 0) override;
        void setSamplerStateImpl(const std::shared_ptr<ISamplerState>& samplerState,
            const int& slot = 0) override;

        void setDepthStencilStateImpl(const std::shared_ptr<IDepthStencilState>& depthStencilState) override;

        void setVertexBufferImpl(const std::shared_ptr<IVertexBuffer>& vertexBuffer,
            const int& slot = 0) override;
        void setVertexBufferOffsetImpl(const int& slot,
            const int& offset) override;

        void setUniformBufferImpl(const std::shared_ptr<IUniformBuffer>& uniformBuffer,
            const int& slot) override;

        void drawPrimitivesImpl(const RenderPrimitives& primitiveType,
            const int& count,
            const int& start) override;
            
        void drawIndexedPrimitivesImpl(const RenderPrimitives& primitiveType,
            const std::shared_ptr<IIndexBuffer>& indexBuffer,
            const int& count,
            const int& start) override;

        void endImpl() override;
    
    private:
        GLGraphicsDevice* const _graphicsDevice;

        std::shared_ptr<GLRenderState> _currentRenderState;
        std::unordered_map<int, std::shared_ptr<IVertexBuffer>> _bufferSlots;
        std::unordered_map<int, int> _bufferOffsets;

        std::unordered_map<int, ResourceHandle<ColorTexture>> _textureSlots;
        std::unordered_map<int, std::shared_ptr<GLSamplerState>> _samplerStateSlots;

        std::unordered_map<int, std::shared_ptr<IUniformBuffer>> _uniformSlots;

        void bindTexturesToSlot();
    };
}

#endif
