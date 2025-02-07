/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLFramebufferManager.hpp>

#include <ff-graphics-opengl/GLTextureImp.hpp>
#include <ff-graphics-opengl/GLGlobalState.hpp>
#include <ff/Console.hpp>

namespace ff {


GLFramebufferManager::GLFramebufferManager(GLGraphicsDevice* const& graphicsDevice)
    :_graphicsDevice(graphicsDevice),
    _drawFBOSlotToAttachment({ 0, 0, 0, 0 }) {
}

void GLFramebufferManager::init() {
    _readFBO = std::make_unique<GLFramebuffer>(_graphicsDevice);
    _drawFBO = std::make_unique<GLFramebuffer>(_graphicsDevice);

    // NOTE: In GL 4.0, the default framebuffer is "readonly", so you can't
    // modify its attachments. We are forced to use the blitting method.
}

GLFramebuffer* GLFramebufferManager::prepareForDraw(std::vector<std::pair<ResourceHandle<ColorTexture>,
        RenderTargetColorAttachmentOptions>> const& colorTargets,
    std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> const& depthTarget) {
    _drawFBO->bindAsDraw();
    std::array<GLenum, 4> drawBuffers = _drawFBO->getDrawBuffers();
    int currentColorAttachment = 0;

    for(int i = 0; i < colorTargets.size(); i++) {
        FF_ASSERT(i < 4, "Max color attachments is 4.");

        GLTextureImp* const glTexImp = static_cast<GLTextureImp*>(colorTargets[i].first->getImp());
        if(glTexImp->isBackBuffer()) {
            drawBuffers[i] = GL_BACK;
            continue;
        }
        drawBuffers[i] = GL_COLOR_ATTACHMENT0 + currentColorAttachment;
        glTexImp->attachToFramebufferAsColor(_drawFBO.get(), currentColorAttachment);
        _drawFBOSlotToAttachment[i] = GL_COLOR_ATTACHMENT0 + currentColorAttachment;
        currentColorAttachment++;
    }
    for(int i = currentColorAttachment; i < 4; i++) {
        _drawFBO->unbindColorAttachment(i);
    }

    if(depthTarget) {
        GLTextureImp* const glTexImp = static_cast<GLTextureImp*>(depthTarget->first->getImp());
        glTexImp->attachToFramebufferAsDepth(_drawFBO.get());
    } else {
        _drawFBO->unbindDepthAttachment();
    }

    _drawFBO->setDrawBuffers(drawBuffers);

    _drawFBO->assertStatus();

    return _drawFBO.get();
}
void GLFramebufferManager::attemptInvalidation(std::vector<std::pair<ResourceHandle<ColorTexture>,
        RenderTargetColorAttachmentOptions>> const& colorTargets,
    std::optional<std::pair<ResourceHandle<DepthTexture>, RenderTargetDepthAttachmentOptions>> const& depthTarget) {
    if((_graphicsDevice->getExtensions() & GLExtensions::ARB_INVALIDATE_SUBDATA) == 0) {
        return;
    }

    std::vector<GLenum> attachmentsToInvalidate;
    for(int i = 0; i < colorTargets.size(); i++) {
        if(colorTargets[i].second.storeAction == RenderTargetStoreAction::DONT_CARE) {
            if(static_cast<GLTextureImp*>(colorTargets[i].first->getImp())->isBackBuffer()) {
                continue;
            }
            attachmentsToInvalidate.push_back(_drawFBOSlotToAttachment[i]);
        }
    }
    if(depthTarget) {
        if(depthTarget->second.storeAction == RenderTargetStoreAction::DONT_CARE) {
            attachmentsToInvalidate.push_back(GL_DEPTH_ATTACHMENT);
        }
    }
    if(attachmentsToInvalidate.size() > 0) {
        FF_GL_CALL(glInvalidateFramebuffer,
            GL_FRAMEBUFFER,
            attachmentsToInvalidate.size(),
            attachmentsToInvalidate.data());
    }
}
void GLFramebufferManager::prepareForCPUTransfer(ResourceHandle<ITexture> const& tex) {
    prepareForCPUTransfer(static_cast<GLTextureImp*>(tex->getImp()));
}
void GLFramebufferManager::prepareForCPUTransfer(GLTextureImp* const& glTexImp) {
    if (glTexImp->isBackBuffer()) {
        if (_graphicsDevice->isGLES()) {
            FF_GL_CALL(glReadBuffer, GL_BACK);
        }
        else {
            _graphicsDevice->getGlobalState()->bindReadFramebuffer(0);
            FF_GL_CALL(glReadBuffer, GL_COLOR_ATTACHMENT0);
        }
    }
    else {
        _readFBO->bindAsRead();
        glTexImp->attachToFramebufferAsColor(_readFBO.get(), 0);
        FF_GL_CALL(glReadBuffer, GL_COLOR_ATTACHMENT0);
        _readFBO->assertStatus();
    }
}

void GLFramebufferManager::prepareForBlitTransfer(ResourceHandle<ITexture> const& src,
    ResourceHandle<ITexture> const& dst) {
    FF_CONSOLE_FATAL("Not implemented.");
}

void GLFramebufferManager::setDrawFBOBlendState(int const& slot, std::optional<BlendState> const& blendState) {
    _drawFBO->setColorBlendState(slot, blendState);
}

}
