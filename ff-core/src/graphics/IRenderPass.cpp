/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/IRenderPass.hpp>

#include <ff/Locator.hpp>
#include <ff/graphics/BufferFormats.hpp>

#include <glm/glm.hpp>

namespace ff {
    IRenderPass::IRenderPass(std::vector<std::pair<ResourceHandle<ColorTexture>,
            RenderTargetColorAttachmentOptions>> const& colorTargets,
        std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> const& depthTarget)
        :_colorTargets(colorTargets),
        _depthTarget(depthTarget) {
        // Validate usage
        for(int i = 0; i < _colorTargets.size(); i++) {
            if(_colorTargets[i].second.loadAction == RenderTargetLoadAction::LOAD) {
                FF_ASSERT((_colorTargets[i].first->getUsage() & TextureUsage::GPU_LOAD) > 0, "Render target in color attachment %i does not have GPU_LOAD usage; the only acceptable load action is `DONT_CARE`.", i);
            }
            if(_colorTargets[i].second.storeAction == RenderTargetStoreAction::STORE) {
                FF_ASSERT((_colorTargets[i].first->getUsage() & TextureUsage::GPU_STORE) > 0, "Render target in color attachment %i does not have GPU_STORE usage; the only acceptable store action is `DONT_CARE`.", i);
            }
        }
        if(_depthTarget != std::nullopt) {
            if(_depthTarget.value().second.loadAction == RenderTargetLoadAction::LOAD) {
                FF_ASSERT((_depthTarget->first->getUsage() & TextureUsage::GPU_LOAD) > 0, "Render target in depth attachment does not have GPU_LOAD usage; the only acceptable load action is `DONT_CARE`.");
            }
            if(_depthTarget.value().second.storeAction == RenderTargetStoreAction::STORE) {
                FF_ASSERT((_depthTarget->first->getUsage() & TextureUsage::GPU_STORE) > 0, "Render target in depth attachment does not have GPU_STORE usage; the only acceptable store action is `DONT_CARE`.");
            }
        }
    }

    void IRenderPass::setState(const std::shared_ptr<IRenderState>& renderState) {
        FF_ASSERT(getColorRenderTargets().size() == renderState->getRenderColorLayerStates().size(),
            "Render color layer states count does not match color render target count.");

        for(int i = 0; i < renderState->getRenderColorLayerStates().size(); i++) {
            FF_ASSERT(getColorRenderTargets()[i].first->getFormat() == renderState->getRenderColorLayerStates()[i].colorBufferFormat,
                "Color buffer format mismatch for Render Target %i: Render Target is `%s`, Render Layer State is `%s`.",
                    i,
                    BufferFormatInfo::get(getColorRenderTargets()[i].first->getFormat()).name,
                    BufferFormatInfo::get(renderState->getRenderColorLayerStates()[i].colorBufferFormat).name);
        }

        if(renderState->getDepthBufferFormat() != std::nullopt) {
            FF_ASSERT(getDepthRenderTarget() != std::nullopt, "Render state has depth buffer format, but no depth render target was attached.");
            FF_ASSERT(getDepthRenderTarget()->first->getFormat()
                == renderState->getDepthBufferFormat(),
                "Depth buffer format mismatch for DepthRenderTarget: Render target is `%s`, Depth Buffer Format is `%s`",
                BufferFormatInfo::get(getDepthRenderTarget()->first->getFormat()).name,
                BufferFormatInfo::get(*renderState->getDepthBufferFormat()).name);
        } else {
            FF_ASSERT(getDepthRenderTarget() == std::nullopt, "Depth render target was attached, but render state does not specify a depth buffer format.");
        }

        setStateImpl(renderState);
    }

    void IRenderPass::setCullMode(const CullMode& cullMode) {
        setCullModeImpl(cullMode);
    }

    void IRenderPass::setViewport(int const& x, int const& y,
        int const& width, int const& height) {
        FF_ASSERT(x >= 0, "Viewport 'x' most be positive.");
        FF_ASSERT(y >= 0, "Viewport 'y' most be positive.");
        glm::ivec2 const bbDimensions = Locator::getGraphicsDevice().getBackBufferDimensions();
        FF_ASSERT((x + width) <= bbDimensions.x
            && (y + height) <= bbDimensions.y,
            "Viewport dimensions are out of range of the back buffer. Viewport: {x: %s, y: %s, w: %s, h: %s}, back buffer: <%s, %s>.",
                x, y, width, height,
                bbDimensions.x, bbDimensions.y);
        setViewportImpl(x, y, width, height);
    }
    void IRenderPass::setScissorRect(int const& x, int const& y,
        int const& width, int const& height) {
        FF_ASSERT(x >= 0, "Scissor rect 'x' most be positive.");
        FF_ASSERT(y >= 0, "Scissor rect 'y' most be positive.");
        glm::ivec2 const bbDimensions = Locator::getGraphicsDevice().getBackBufferDimensions();
        FF_ASSERT((x + width) <= bbDimensions.x
            && (y + height) <= bbDimensions.y,
            "Scissor rect dimensions are out of range of the back buffer. Scissor rect: {x: %s, y: %s, w: %s, h: %s}, back buffer: <%s, %s>.",
                x, y, width, height,
                bbDimensions.x, bbDimensions.y);
        setScissorRectImpl(x, y, width, height);
    }

    void IRenderPass::setTexture(const ResourceHandle<ColorTexture>& texture,
        const int& slot) {
        FF_ASSERT((texture->getUsage() & TextureUsage::GPU_SAMPLE) > 0, "Texture does not have GPU_SAMPLE usage.");
        setTextureImpl(texture, slot);
    }
    void IRenderPass::setSamplerState(const std::shared_ptr<ISamplerState>& samplerState,
        const int& slot) {
        setSamplerStateImpl(samplerState, slot);
    }

    void IRenderPass::setDepthStencilState(const std::shared_ptr<IDepthStencilState>& depthStencilState) {
        setDepthStencilStateImpl(depthStencilState);
    }

    void IRenderPass::setVertexBuffer(const std::shared_ptr<IVertexBuffer>& vertexBuffer,
        const int& slot) {
        setVertexBufferImpl(vertexBuffer, slot);
    }
    void IRenderPass::setVertexBufferOffset(const int& slot,
        const int& offset) {
        setVertexBufferOffsetImpl(slot, offset);
    }

    void IRenderPass::setUniformBuffer(const std::shared_ptr<IUniformBuffer>& uniformBuffer,
        const int& slot) {
        setUniformBufferImpl(uniformBuffer, slot);
    }

    void IRenderPass::drawPrimitives(const RenderPrimitives& primitiveType,
        const int& count,
        const int& start) {
        drawPrimitivesImpl(primitiveType, count, start);
    }
        
    void IRenderPass::drawIndexedPrimitives(const RenderPrimitives& primitiveType,
        const std::shared_ptr<IIndexBuffer>& indexBuffer,
        const int& count,
        const int& start) {
        drawIndexedPrimitivesImpl(primitiveType, indexBuffer, count, start);
    }

    void IRenderPass::end() {
        endImpl();
    }

    std::vector<std::pair<ResourceHandle<ColorTexture>,
        RenderTargetColorAttachmentOptions>>& IRenderPass::getColorRenderTargets() {
        return _colorTargets;
    }
    std::optional<std::pair<ResourceHandle<DepthTexture>,
        RenderTargetDepthAttachmentOptions>>& IRenderPass::getDepthRenderTarget() {
        return _depthTarget;
    }
}
