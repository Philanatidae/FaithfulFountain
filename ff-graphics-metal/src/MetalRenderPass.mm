/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-metal/MetalRenderPass.hmm>

#include <ff-graphics-metal/MetalConstants.hmm>

#include <ff-graphics-metal/MetalShader.hmm>

#include <ff-graphics-metal/MetalSamplerState.hmm>

#include <ff-graphics-metal/MetalVertexDescriptor.hmm>

#include <ff-graphics-metal/MetalDepthStencilState.hmm>

#include <ff-graphics-metal/MetalRenderState.hmm>

#include <ff-graphics-metal/MetalBuffer.hmm>

#include <ff-graphics-metal/MetalTextureImp.hmm>

#include <ff/CVars.hpp>
#include <ff/graphics/Color.hpp>
#include <memory>

namespace ff {
    MetalRenderPass::MetalRenderPass(MetalGraphicsDevice* const& graphicsDevice,
        std::vector<std::pair<ResourceHandle<ColorTexture>,
            RenderTargetColorAttachmentOptions>> const& colorTargets,
        std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> const& depthTarget)
        :IRenderPass(colorTargets, depthTarget),
        _graphicsDevice(graphicsDevice) {

        MTLRenderPassDescriptor* renderPassDescriptor = [MTLRenderPassDescriptor new];

        for(int i = 0; i < getColorRenderTargets().size(); i++) {
            MetalTextureImp* const metalImp = static_cast<MetalTextureImp*>(getColorRenderTargets()[i].first->getImp());
            metalImp->bindToRenderPassDescriptorAsColor(renderPassDescriptor,
                getColorRenderTargets()[i].first->getFormat(),
                getColorRenderTargets()[i].second,
                i);
        }

        if(getDepthRenderTarget() != std::nullopt) {
            MetalTextureImp* const metalImp = static_cast<MetalTextureImp*>(getDepthRenderTarget()->first->getImp());
            metalImp->bindToRenderPassDescriptorAsDepth(renderPassDescriptor,
                getDepthRenderTarget().value().first->getFormat(),
                getDepthRenderTarget()->second);
        }

        // Create encoder
        _commandEncoder = [graphicsDevice->getCurrentCommandBuffer() renderCommandEncoderWithDescriptor:renderPassDescriptor];
        // Standard: counter-clockwise is front
        [_commandEncoder setFrontFacingWinding:MTLWindingCounterClockwise];
    }

    MetalRenderPass::~MetalRenderPass() {
    }

    id<MTLRenderCommandEncoder> MetalRenderPass::getMetalCommandEncoder() {
        return _commandEncoder;
    }

    void MetalRenderPass::setStateImpl(const std::shared_ptr<IRenderState>& renderState) {
        auto metalRenderState = std::static_pointer_cast<MetalRenderState>(renderState);
        
        // @todo Cache render pipeline state so we don't set it multiple times.

        [_commandEncoder setRenderPipelineState:metalRenderState->getMetalRenderPipelineState()];

        _currentShader = std::static_pointer_cast<MetalShader>(metalRenderState->getShader());
        
        for(auto& pair : _boundTextures) {
            setTexture(pair.second, pair.first);
        }
        for(auto& pair : _boundSamplerStates) {
            setSamplerState(pair.second, pair.first);
        }
        for(auto& pair : _boundUniformBuffers) {
            setUniformBuffer(pair.second, pair.first);
        }
    }

    void MetalRenderPass::setCullModeImpl(const CullMode& cullMode) {
        switch(cullMode) {
            case CullMode::CLOCKWISE:
                [_commandEncoder setCullMode:MTLCullModeBack];
                break;
            case CullMode::COUNTER_CLOCKWISE:
                [_commandEncoder setCullMode:MTLCullModeFront];
                break;
            case CullMode::NONE:
            default:
                [_commandEncoder setCullMode:MTLCullModeNone];
                break;
        }
    }

    void MetalRenderPass::setViewportImpl(int const& x, int const& y,
        int const& width, int const& height) {
        MTLViewport viewport = {
            .originX = (double)x,
            .originY = (double)y,
            .width = (double)width,
            .height = (double)height,
            .znear = 0,
            .zfar = 1
        };
        [_commandEncoder setViewport:viewport];
    }
    void MetalRenderPass::setScissorRectImpl(int const& x, int const& y,
        int const& width, int const& height) {
        MTLScissorRect scissorRect = {
            .x = NSUInteger(x),
            .y = NSUInteger(y),
            .width = NSUInteger(width),
            .height = NSUInteger(height)
        };
        [_commandEncoder setScissorRect:scissorRect];
    }
    
    void MetalRenderPass::setTextureImpl(const ResourceHandle<ColorTexture> &texture,
        const int &slot) {
        
        MetalTextureImp* metalTextureImp = static_cast<MetalTextureImp*>(texture->getImp());

        _boundTextures[slot] = texture;
        if(_currentShader) {
            if(_currentShader->isImageSlotInVertexFunction(slot)
                && _vertexTextures[slot] != texture) {
                [_commandEncoder setVertexTexture:metalTextureImp->getMTLTexture() atIndex:(NSUInteger)slot];
                _vertexTextures[slot] = texture;
            }
            if(_currentShader->isImageSlotInFragmentFunction(slot)
                && _fragmentTextures[slot] != texture) {
                [_commandEncoder setFragmentTexture:metalTextureImp->getMTLTexture() atIndex:(NSUInteger)slot];
                _fragmentTextures[slot] = texture;
            }
        }
    }
    void MetalRenderPass::setSamplerStateImpl(const std::shared_ptr<ISamplerState>& samplerState,
        const int& slot) {
        auto metalSamplerState = std::static_pointer_cast<MetalSamplerState>(samplerState);

        _boundSamplerStates[slot] = metalSamplerState;
        if(_currentShader) {
            if(_currentShader->isImageSlotInVertexFunction(slot)
                && _vertexSamplerStates[slot] != metalSamplerState) {
                [_commandEncoder setVertexSamplerState:metalSamplerState->getSamplerState() atIndex:(NSUInteger)slot];
                _vertexSamplerStates[slot] = metalSamplerState;
            }
            if(_currentShader->isImageSlotInFragmentFunction(slot)
                && _fragmentSamplerStates[slot] != metalSamplerState) { // @todo Metal is currently complaining that the fragment sampler state is being bound twice; this is because we are not taking into account Metal sampler state is per-encoder when we are assuming it is per-shader (which is because of OpenGL shenanigans IIRC).
                [_commandEncoder setFragmentSamplerState:metalSamplerState->getSamplerState() atIndex:(NSUInteger)slot];
                _fragmentSamplerStates[slot] = metalSamplerState;
            }
        }
    }

    void MetalRenderPass::setDepthStencilStateImpl(const std::shared_ptr<IDepthStencilState>& depthStencilState) {
        auto& metalDepthStencilState = static_cast<MetalDepthStencilState&>(*depthStencilState);
        [_commandEncoder setDepthStencilState:metalDepthStencilState.getDepthStencilState()];
    }

    void MetalRenderPass::setVertexBufferImpl(const std::shared_ptr<IVertexBuffer>& vertexBuffer,
        const int& slot) {
        MetalBufferImp<VertexBufferImp>& vertexBufferImp = *static_cast<MetalBufferImp<VertexBufferImp>*>(vertexBuffer->getImp());
        // Start backwards; this prevents collisions between uniform buffers and vertex buffers
        [_commandEncoder setVertexBuffer:vertexBufferImp.getMetalBuffer().getBuffer() offset:0 atIndex:METAL_MAX_BUFFER_COUNT - slot]; // @todo offset?
    }
    void MetalRenderPass::setVertexBufferOffsetImpl(const int& slot,
        const int& offset) {
        // Vertex buffers start at 30, 29, 28...
        [_commandEncoder setVertexBufferOffset:offset atIndex:METAL_MAX_BUFFER_COUNT - slot];
    }

    void MetalRenderPass::setUniformBufferImpl(const std::shared_ptr<IUniformBuffer>& uniformBuffer,
        const int& slot) {
        MetalBufferImp<UniformBufferImp>& uniformBufferImp = *static_cast<MetalBufferImp<UniformBufferImp>*>(uniformBuffer->getImp());

        _boundUniformBuffers[slot] = uniformBuffer;
        if(_currentShader) {
            if(_currentShader->isUniformBufferSlotInVertexFunction(slot)
                && _vertexUniformBuffers[slot] != uniformBuffer) {
                [_commandEncoder setVertexBuffer:uniformBufferImp.getMetalBuffer().getBuffer() offset:0 atIndex:slot]; // @todo offset?
                _vertexUniformBuffers[slot] = uniformBuffer;
            }
            if(_currentShader->isUniformBufferSlotInFragmentFunction(slot)
                && _fragmentUniformBuffers[slot] != uniformBuffer) {
                [_commandEncoder setFragmentBuffer:uniformBufferImp.getMetalBuffer().getBuffer() offset:0 atIndex:slot]; // @todo offset?
                _fragmentUniformBuffers[slot] = uniformBuffer;
            }
        }
    }

    void MetalRenderPass::drawPrimitivesImpl(const RenderPrimitives& primitiveType,
        const int& count,
        const int& start) {
        if(count == 0) {
            return;
        }
        switch(primitiveType) {
            case RenderPrimitives::TRIANGLE_LIST:
                [_commandEncoder drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:start vertexCount:count];
                break;
            case RenderPrimitives::TRIANGLE_STRIP:
                [_commandEncoder drawPrimitives:MTLPrimitiveTypeTriangleStrip vertexStart:start vertexCount:count];
                break;
            case RenderPrimitives::LINE_STRIP:
                [_commandEncoder drawPrimitives:MTLPrimitiveTypeLineStrip vertexStart:start vertexCount:count];
                break;
            default:
                FF_CONSOLE_FATAL("Unsupported primitive type.");
        }
    }
        
    void MetalRenderPass::drawIndexedPrimitivesImpl(const RenderPrimitives& primitiveType,
        const std::shared_ptr<IIndexBuffer>& indexBuffer,
        const int& count,
        const int& start) {
        if(count == 0) {
            return;
        }

        MTLIndexType indexType;
        switch(indexBuffer->getIndexType()) {
        case IndexType::UINT16:
            indexType = MTLIndexTypeUInt16;
            break;
        case IndexType::UINT32:
            indexType = MTLIndexTypeUInt32;
            break;
        default:
            FF_CONSOLE_FATAL("Unknown index type.");
        }

        MetalBufferImp<IndexBufferImp>& indexBufferImp = *static_cast<MetalBufferImp<IndexBufferImp>*>(indexBuffer->getImp());
        switch(primitiveType) {
            case RenderPrimitives::TRIANGLE_LIST:
                [_commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle indexCount:count indexType:indexType indexBuffer:indexBufferImp.getMetalBuffer().getBuffer() indexBufferOffset:start * indexBuffer->getElementSize()];
                break;
            case RenderPrimitives::TRIANGLE_STRIP:
                [_commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangleStrip indexCount:count indexType:indexType indexBuffer:indexBufferImp.getMetalBuffer().getBuffer() indexBufferOffset:start * indexBuffer->getElementSize()];
                break;
            case RenderPrimitives::LINE_STRIP:
                [_commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeLineStrip indexCount:count indexType:indexType indexBuffer:indexBufferImp.getMetalBuffer().getBuffer() indexBufferOffset:start * indexBuffer->getElementSize()];
                break;
            default:
                FF_CONSOLE_FATAL("Unsupported primitive type.");
                break;
        }
    }

    void MetalRenderPass::endImpl() {
        [_commandEncoder endEncoding];
    }
}
