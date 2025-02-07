/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLRenderPass.hpp>

#include <ff/Console.hpp>
#include <ff/CVars.hpp>

#include <ff/graphics/Color.hpp>

#include <ff-graphics-opengl/GLIndexBufferImp.hpp>
#include <ff-graphics-opengl/GLUniformBufferImp.hpp>
#include <ff-graphics-opengl/GLDepthStencilState.hpp>
#include <ff-graphics-opengl/GLVertexBufferImp.hpp>
#include <ff-graphics-opengl/GLRenderState.hpp>
#include <ff-graphics-opengl/GLTexture.hpp>
#include <ff-graphics-opengl/GLSamplerState.hpp>
#include <ff-graphics-opengl/GLTextureImp.hpp>

#include <ff-graphics-opengl/GLGlobalState.hpp>
#include <ff-graphics-opengl/GLFramebufferManager.hpp>

#include <memory>
#include <array>

#include <ff/Locator.hpp>

namespace ff {
    GLRenderPass::GLRenderPass(GLGraphicsDevice* const& graphicsDevice,
        std::vector<std::pair<ResourceHandle<ColorTexture>,
            RenderTargetColorAttachmentOptions>> const& colorTargets,
        std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> const& depthTarget)
        :IRenderPass(colorTargets, depthTarget),
        _graphicsDevice(graphicsDevice) {
        _graphicsDevice->getGlobalState()->setScissorEnabled(false);
        _graphicsDevice->getGlobalState()->setDepthEnabled(false);
        
        _graphicsDevice->getFramebufferManager()->prepareForDraw(colorTargets, depthTarget);
        for(int i = 0; i < colorTargets.size(); i++) {
            if(colorTargets[i].second.loadAction == RenderTargetLoadAction::CLEAR
                || colorTargets[i].second.loadAction == RenderTargetLoadAction::DONT_CARE) {
                // @todo If there is scissor and we are DONT_CARE, don't clear. This is recommended on Mali's docs.
                BufferFormatInfo_t const formatInfo = BufferFormatInfo::get(colorTargets[i].first->getFormat());
                switch (formatInfo.baseType) {
                case FormatBaseType::FLOAT:
                    {
                        std::array<float, 4> col({
                                (float)colorTargets[i].second.clearColor.r,
                                (float)colorTargets[i].second.clearColor.g,
                                (float)colorTargets[i].second.clearColor.b,
                                (float)colorTargets[i].second.clearColor.a
                            });
                        FF_GL_CALL(glClearBufferfv,
                            GL_COLOR,
                            i,
                            col.data());
                    }
                    break;
                case FormatBaseType::INT:
                    {
                        std::array<int32_t, 4> col({
                            (int32_t)colorTargets[i].second.clearColor.r,
                            (int32_t)colorTargets[i].second.clearColor.g,
                            (int32_t)colorTargets[i].second.clearColor.b,
                            (int32_t)colorTargets[i].second.clearColor.a
                            });
                        FF_GL_CALL(glClearBufferiv,
                            GL_COLOR,
                            i,
                            (GLint*)col.data());
                    }
                    break;
                case FormatBaseType::UINT:
                    {
                        std::array<uint32_t, 4> col({
                            (uint32_t)colorTargets[i].second.clearColor.r,
                            (uint32_t)colorTargets[i].second.clearColor.g,
                            (uint32_t)colorTargets[i].second.clearColor.b,
                            (uint32_t)colorTargets[i].second.clearColor.a
                            });
                        FF_GL_CALL(glClearBufferuiv,
                            GL_COLOR,
                            i,
                            (GLuint*)col.data());
                    }
                    break;
                default:
                    FF_CONSOLE_FATAL("Invalid format base type.");
                    break;
                }
            }
        }
        if(depthTarget) {
            if(depthTarget->second.loadAction == RenderTargetLoadAction::CLEAR
                || depthTarget->second.loadAction == RenderTargetLoadAction::DONT_CARE) {
                // @todo If there is scissor and we are DONT_CARE, don't clear. This is recommended on Mali's docs.
                
                // @todo Can glClearBufferiv/uiv be used for depth?
                FF_GL_CALL(glClearBufferfv,
                    GL_DEPTH,
                    0,
                    &depthTarget->second.clearDepth);
            }
        }
    }
    
    void GLRenderPass::setStateImpl(const std::shared_ptr<IRenderState>& renderState) {
        _currentRenderState = std::static_pointer_cast<GLRenderState>(renderState);

        GLShader& shader = static_cast<GLShader&>(*_currentRenderState->getShader());
        shader.bind();

        for (auto& uniformSlot : _uniformSlots) {
            setUniformBuffer(uniformSlot.second,
                uniformSlot.first);
        }
    }

    void GLRenderPass::setCullModeImpl(const CullMode& cullMode) {
        // CW/CCW are flipped because we are flipping-Y in
        // the shader/projection matrix.

        _graphicsDevice->getGlobalState()->setCullFace(GL_FRONT);
        switch(cullMode) {
        case CullMode::COUNTER_CLOCKWISE:
            _graphicsDevice->getGlobalState()->setFrontFace(GL_CCW);
            _graphicsDevice->getGlobalState()->setCullFaceEnabled(true);
            break;
        case CullMode::CLOCKWISE:
            _graphicsDevice->getGlobalState()->setFrontFace(GL_CW);
            _graphicsDevice->getGlobalState()->setCullFaceEnabled(true);
            break;
        case CullMode::NONE:
            _graphicsDevice->getGlobalState()->setCullFaceEnabled(false);
            break;
        }
    }

    void GLRenderPass::setViewportImpl(int const& x, int const& y,
        int const& width, int const& height) {
        FF_GL_CALL(glViewport,
            x, y,
            width, height)
    }
    void GLRenderPass::setScissorRectImpl(int const& x, int const& y,
        int const& width, int const& height) {
        FF_GL_CALL(glScissor,
            x,
            getColorRenderTargets()[0].first->getHeight() - height - y,
            width, height);
        _graphicsDevice->getGlobalState()->setScissorEnabled(true);
    }

    void GLRenderPass::setTextureImpl(const ResourceHandle<ColorTexture>& texture,
        const int &slot) {
        GLTextureImp* glTexImp = static_cast<GLTextureImp*>(texture->getImp());
        auto textureSlotIt = _textureSlots.find(slot);
        if(textureSlotIt == _textureSlots.end()) {
            _textureSlots.emplace(slot, texture);
        } else {
            textureSlotIt->second = texture;
        }
    }
    void GLRenderPass::setSamplerStateImpl(const std::shared_ptr<ISamplerState>& samplerState,
        const int& slot) {
        // When a sampler state is set, update samplers for textures bound.
        // When a texture is bound, update texture with samplers for the same
        // slot.
        std::shared_ptr<GLSamplerState> glSamplerState = std::static_pointer_cast<GLSamplerState>(samplerState);
        auto samplerStateSlotIt = _samplerStateSlots.find(slot);
        if (samplerStateSlotIt == _samplerStateSlots.end()) {
            _samplerStateSlots.emplace(slot, glSamplerState);
        }
        else {
            samplerStateSlotIt->second = glSamplerState;
        }
    }

    void GLRenderPass::setDepthStencilStateImpl(const std::shared_ptr<IDepthStencilState>& depthStencilState) {
        GLDepthStencilState& glDepthStencilState = static_cast<GLDepthStencilState&>(*depthStencilState);
        glDepthStencilState.bind();
    }

    void GLRenderPass::setVertexBufferImpl(const std::shared_ptr<IVertexBuffer>& vertexBuffer,
        const int& slot) {
        // When a vertex buffer is set, the vertex descriptors
        // need to be re-bound for this slot. When vertex descriptors
        // are set, all vertex buffers need to be re-bound to the updated
        // descriptors.
        auto bufferSlotIt = _bufferSlots.find(slot);
        if (bufferSlotIt == _bufferSlots.end()) {
            _bufferSlots.emplace(slot, vertexBuffer);
        }
        else {
            bufferSlotIt->second = vertexBuffer;
        }
    }
    void GLRenderPass::setVertexBufferOffsetImpl(const int& slot,
        const int& offset) {
        _bufferOffsets[slot] = offset;
    }

    void GLRenderPass::setUniformBufferImpl(const std::shared_ptr<IUniformBuffer>& uniformBuffer,
        const int& slot) {
        GLUniformBufferImp* bufferImp = static_cast<GLUniformBufferImp*>(uniformBuffer->getImp());
        bufferImp->bindToLocation(slot);
        // @todo Need to remember this and rebind when the render state (shader) changes; it sets onto
        // the currently attached shader.
        auto uniformSlotIt = _uniformSlots.find(slot);
        if (uniformSlotIt == _uniformSlots.end()) {
            _uniformSlots.emplace(slot, uniformBuffer);
        }
        else {
            uniformSlotIt->second = uniformBuffer;
        }
    }

    void GLRenderPass::drawPrimitivesImpl(const RenderPrimitives& primitiveType,
        const int& count,
        const int& start) {
        bindTexturesToSlot();
        _currentRenderState->prepareForDrawCall(_bufferSlots, _bufferOffsets, nullptr);

        GLenum mode;
        // @todo Move to helper method
        switch(primitiveType) {
            case RenderPrimitives::TRIANGLE_LIST:
                mode = GL_TRIANGLES;
                break;
            case RenderPrimitives::TRIANGLE_STRIP:
                mode = GL_TRIANGLE_STRIP;
                break;
            case RenderPrimitives::LINE_STRIP:
                mode = GL_LINE_STRIP;
                break;
            default:
                FF_CONSOLE_ERROR("Unknown primitive type to draw.");
                break;
        }
        FF_GL_CALL(glDrawArrays, mode, start, count);
    }
        
    void GLRenderPass::drawIndexedPrimitivesImpl(const RenderPrimitives& primitiveType,
        const std::shared_ptr<IIndexBuffer>& indexBuffer,
        const int& count,
        const int& start) {
        bindTexturesToSlot();
        _currentRenderState->prepareForDrawCall(_bufferSlots, _bufferOffsets, indexBuffer.get());

        GLenum mode;
        // @todo Move to helper method
        switch(primitiveType) {
            case RenderPrimitives::TRIANGLE_LIST:
                mode = GL_TRIANGLES;
                break;
            case RenderPrimitives::TRIANGLE_STRIP:
                mode = GL_TRIANGLE_STRIP;
                break;
            case RenderPrimitives::LINE_STRIP:
                mode = GL_LINE_STRIP;
                break;
            default:
                FF_CONSOLE_ERROR("Unknown primitive type to draw.");
                break;
        }

        GLenum type = 0;
        size_t typeSize = 0;
        switch(indexBuffer->getIndexType()) {
        case IndexType::UINT16:
            type = GL_UNSIGNED_SHORT;
            typeSize = sizeof(typename IndexTypeUnderlyingType<IndexType::UINT16>::type);
            break;
        case IndexType::UINT32:
            type = GL_UNSIGNED_INT;
            typeSize = sizeof(typename IndexTypeUnderlyingType<IndexType::UINT32>::type);
            break;
        default:
            FF_CONSOLE_FATAL("Unknown index type.");
            break;
        }

        FF_GL_CALL(glDrawElements, mode,
            count,
            type,
            (void*)(start * typeSize));
    }

    void GLRenderPass::endImpl() {
        _graphicsDevice->getFramebufferManager()->attemptInvalidation(getColorRenderTargets(),
            getDepthRenderTarget());
        
        if(CVars::get<bool>("graphics_gl_print_api_calls")) {
            FF_CONSOLE_LOG("End render pass.");
        }
    }

    void GLRenderPass::bindTexturesToSlot() {
        for(auto pair : _textureSlots) {
            GLTextureImp* glTexImp = static_cast<GLTextureImp*>(pair.second->getImp());
            glTexImp->attachToSamplerSlot(pair.first);

            // @todo Throw an erorr if sampler state does not exist for this slot. Move this error to FF.
            
            auto samplerStateIt = _samplerStateSlots.find(pair.first);
            if(samplerStateIt != _samplerStateSlots.end()) {
                samplerStateIt->second->bindToTexture(pair.second);
            }
        }
    }
}
