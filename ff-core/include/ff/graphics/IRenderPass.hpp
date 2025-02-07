/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_IRENDER_PASS_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_IRENDER_PASS_HPP

#include <ff/resources/ResourceHandle.hpp>
#include <memory>
#include <utility>

#include <ff/graphics/Texture.hpp>
#include <ff/graphics/IRenderState.hpp>
#include <ff/graphics/ISamplerState.hpp>
#include <ff/graphics/IDepthStencilState.hpp>
#include <ff/graphics/VertexBuffer.hpp>
#include <ff/graphics/UniformBuffer.hpp>
#include <ff/graphics/IndexBuffer.hpp>
#include <ff/graphics/Color.hpp>
#include <ff/graphics/ClearColor.hpp>

namespace ff {
    enum class RenderPrimitives {
        TRIANGLE_LIST,
        TRIANGLE_STRIP,
        LINE_STRIP
    };

    enum class AttachmentType {
        FRAGMENT,
        VERTEX,
        BOTH
    };

    enum class CullMode {
        NONE,
        CLOCKWISE,
        COUNTER_CLOCKWISE
    };

    enum class RenderTargetLoadAction {
        CLEAR,
        LOAD,
        DONT_CARE
    };
    enum class RenderTargetStoreAction {
        STORE,
        DONT_CARE
    };

    struct RenderTargetColorAttachmentOptions {
        RenderTargetLoadAction loadAction = RenderTargetLoadAction::CLEAR;
        RenderTargetStoreAction storeAction = RenderTargetStoreAction::STORE;

        ff::ClearColor clearColor = ff::ClearColor(0, 0, 0, 1);
    };
    struct RenderTargetDepthAttachmentOptions {
        RenderTargetLoadAction loadAction = RenderTargetLoadAction::CLEAR;
        RenderTargetStoreAction storeAction = RenderTargetStoreAction::STORE;

        float clearDepth = 0;
    };

    class IRenderPass {
    public:
        IRenderPass(std::vector<std::pair<ResourceHandle<ColorTexture>,
                RenderTargetColorAttachmentOptions>> const& colorTargets,
            std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> const& depthTarget);
        virtual ~IRenderPass() = default;

        void setState(const std::shared_ptr<IRenderState>& renderState);

        void setCullMode(const CullMode& cullMode);

        void setViewport(int const& x, int const& y,
            int const& width, int const& height);
        void setScissorRect(int const& x, int const& y,
            int const& width, int const& height);

        void setTexture(const ResourceHandle<ColorTexture>& texture,
            const int& slot);
        void setSamplerState(const std::shared_ptr<ISamplerState>& samplerState,
            const int& slot);

        void setDepthStencilState(const std::shared_ptr<IDepthStencilState>& depthStencilState);

        void setVertexBuffer(const std::shared_ptr<IVertexBuffer>& vertexBuffer,
            const int& slot);
        void setVertexBufferOffset(const int& slot,
            const int& offset);

        void setUniformBuffer(const std::shared_ptr<IUniformBuffer>& uniformBuffer,
            const int& slot);

        void drawPrimitives(const RenderPrimitives& primitiveType,
            const int& count,
            const int& start = 0);
            
        void drawIndexedPrimitives(const RenderPrimitives& primitiveType,
            const std::shared_ptr<IIndexBuffer>& indexBuffer,
            const int& count,
            const int& start = 0);

        void end();

    protected:
        std::vector<std::pair<ResourceHandle<ColorTexture>,
            RenderTargetColorAttachmentOptions>>& getColorRenderTargets();
        std::optional<std::pair<ResourceHandle<DepthTexture>,
            RenderTargetDepthAttachmentOptions>>& getDepthRenderTarget();

        virtual void setStateImpl(const std::shared_ptr<IRenderState>& renderState) = 0;

        virtual void setCullModeImpl(const CullMode& cullMode) = 0;

        virtual void setViewportImpl(int const& x, int const& y,
            int const& width, int const& height) = 0;
        virtual void setScissorRectImpl(int const& x, int const& y,
            int const& width, int const& height) = 0;

        virtual void setTextureImpl(const ResourceHandle<ColorTexture>& texture,
                                    const int& slot) = 0;
        
        virtual void setSamplerStateImpl(const std::shared_ptr<ISamplerState>& samplerState,
            const int& slot) = 0;
        
        virtual void setDepthStencilStateImpl(const std::shared_ptr<IDepthStencilState>& depthStencilState) = 0;

        virtual void setVertexBufferImpl(const std::shared_ptr<IVertexBuffer>& vertexBuffer,
            const int& slot) = 0;
        virtual void setVertexBufferOffsetImpl(const int& slot,
            const int& offset) = 0;
        
        virtual void setUniformBufferImpl(const std::shared_ptr<IUniformBuffer>& uniformBuffer,
            const int& slot) = 0;
        
        virtual void drawPrimitivesImpl(const RenderPrimitives& primitiveType,
            const int& count,
            const int& start) = 0;
        virtual void drawIndexedPrimitivesImpl(const RenderPrimitives& primitiveType,
            const std::shared_ptr<IIndexBuffer>& indexBuffer,
            const int& count,
            const int& start) = 0;
        
        virtual void endImpl() = 0;

    private:
        std::vector<std::pair<ResourceHandle<ColorTexture>,
            RenderTargetColorAttachmentOptions>> _colorTargets;
        std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> _depthTarget;
    };

    class NullRenderPass : public IRenderPass {
    public:
        NullRenderPass(std::vector<std::pair<ResourceHandle<ColorTexture>,
                RenderTargetColorAttachmentOptions>> const& colorTargets,
            std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> const& depthTarget)
            :IRenderPass(colorTargets,
                depthTarget) {
        }
        virtual ~NullRenderPass() = default;
    
    protected:
        void setStateImpl(const std::shared_ptr<IRenderState>& renderState) override {
        }

        void setCullModeImpl(const CullMode& cullMode) override {
        }

        void setViewportImpl(int const& x, int const& y,
            int const& width, int const& height) override {
        }
        void setScissorRectImpl(int const& x, int const& y,
            int const& width, int const& height) override {
        }

        void setTextureImpl(const ResourceHandle<ColorTexture> &texture,
                                const int &slot = 0) override {                        
        }
        void setSamplerStateImpl(const std::shared_ptr<ISamplerState>& samplerState,
            const int& slot = 0) override {
        }

        void setDepthStencilStateImpl(const std::shared_ptr<IDepthStencilState>& depthStencilState) override {
        }

        void setVertexBufferImpl(const std::shared_ptr<IVertexBuffer>& vertexBuffer,
            const int& slot = 0) override {
        }

        void setUniformBufferImpl(const std::shared_ptr<IUniformBuffer>& uniformBuffer,
            const int& slot) override {    
        }
        void setVertexBufferOffsetImpl(const int& slot,
            const int& offset) override {
        }

        void drawPrimitivesImpl(const RenderPrimitives& primitiveType,
            const int& count,
            const int& start = 0) override {
        }
            
        void drawIndexedPrimitivesImpl(const RenderPrimitives& primitiveType,
            const std::shared_ptr<IIndexBuffer>& indexBuffer,
            const int& count,
            const int& start = 0) override {
        }

        void endImpl() override {
        }
    };
}

#endif
