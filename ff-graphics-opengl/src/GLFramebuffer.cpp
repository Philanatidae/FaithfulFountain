/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLFramebuffer.hpp>

#include <ff-graphics-opengl/GLBlendStateHelpers.hpp>

#include <ff-graphics-opengl/GLGlobalState.hpp>

namespace ff {

GLFramebuffer::GLFramebuffer(GLGraphicsDevice* const& graphicsDevice, GLuint const& fbo)
    :_graphicsDevice(graphicsDevice),
    _fbo(fbo),
    _owned(false),
    _bindTarget(GL_DRAW_FRAMEBUFFER),
    _colorAttachments({
        std::pair<GLenum, GLuint>(0, 0),
        std::pair<GLenum, GLuint>(0, 0),
        std::pair<GLenum, GLuint>(0, 0),
        std::pair<GLenum, GLuint>(0, 0)
    }),
    _colorBlendStates({
        BlendState(BlendOperation::Add, BlendFactor::One, BlendFactor::Zero,
            BlendOperation::Add, BlendFactor::One, BlendFactor::Zero),
        BlendState(BlendOperation::Add, BlendFactor::One, BlendFactor::Zero,
            BlendOperation::Add, BlendFactor::One, BlendFactor::Zero),
        BlendState(BlendOperation::Add, BlendFactor::One, BlendFactor::Zero,
            BlendOperation::Add, BlendFactor::One, BlendFactor::Zero),
        BlendState(BlendOperation::Add, BlendFactor::One, BlendFactor::Zero,
            BlendOperation::Add, BlendFactor::One, BlendFactor::Zero),
    }),
    _colorBlendEnable({ false, false, false, false }),
    _depthAttachment(0, 0),
    _drawBuffers({ GL_NONE, GL_NONE, GL_NONE, GL_NONE }) {
}
GLFramebuffer::GLFramebuffer(GLGraphicsDevice* const& graphicsDevice)
    :GLFramebuffer(graphicsDevice, 0) {
    FF_GL_CALL(glGenFramebuffers, 1, &_fbo);
    _owned = true;
}
GLFramebuffer::~GLFramebuffer() {
    if (_owned) {
        FF_GL_CALL(glDeleteFramebuffers, 1, &_fbo);
    }
}

std::array<GLenum, 4> GLFramebuffer::getDrawBuffers() const {
    return _drawBuffers;
}

void GLFramebuffer::bindAsDraw() {
    _graphicsDevice->getGlobalState()->bindDrawFramebuffer(_fbo);
    _bindTarget = GL_DRAW_FRAMEBUFFER;
}
void GLFramebuffer::bindAsRead() {
    _graphicsDevice->getGlobalState()->bindReadFramebuffer(_fbo);
    _bindTarget = GL_READ_FRAMEBUFFER;
}

void GLFramebuffer::attachColorAttachmentAsTexture2D(int const& slot,
    GLuint const& tex) {
    if(_colorAttachments[slot].first == GL_TEXTURE_2D
        && _colorAttachments[slot].second == tex) {
        return;
    }

    verifyBound();

    _colorAttachments[slot].first = GL_TEXTURE_2D;
    _colorAttachments[slot].second = tex;
    FF_GL_CALL(glFramebufferTexture2D,
        _bindTarget,
        GL_COLOR_ATTACHMENT0 + slot,
        GL_TEXTURE_2D,
        tex,
        0);
}
void GLFramebuffer::attachColorAttachmentAsRenderBuffer(int const& slot,
    GLuint const& rb) {
    if(_colorAttachments[slot].first == GL_RENDERBUFFER
        && _colorAttachments[slot].second == rb) {
        return;
    }

    verifyBound();

    _colorAttachments[slot].first = GL_RENDERBUFFER;
    _colorAttachments[slot].second = rb;
    FF_GL_CALL(glFramebufferRenderbuffer,
        _bindTarget,
        GL_COLOR_ATTACHMENT0 + slot,
        GL_RENDERBUFFER,
        rb);
}
void GLFramebuffer::unbindColorAttachment(int const& slot) {
    if(_colorAttachments[slot].second == 0) {
        return;
    }

    verifyBound();

    _colorAttachments[slot].second = 0;
    switch(_colorAttachments[slot].first) {
    case GL_TEXTURE_2D:
        FF_GL_CALL(glFramebufferTexture2D,
            _bindTarget,
            GL_COLOR_ATTACHMENT0 + slot,
            GL_TEXTURE_2D,
            0,
            0);
        break;
    case GL_RENDERBUFFER:
        FF_GL_CALL(glFramebufferRenderbuffer,
            _bindTarget,
            GL_COLOR_ATTACHMENT0 + slot,
            GL_RENDERBUFFER,
            0);
        break;
    default:
        FF_CONSOLE_FATAL("Unknown attachment type.");
        break;
    }
}

void GLFramebuffer::attachDepthAttachmentAsTexture2D(GLuint const& tex) {
    if(_depthAttachment.first == GL_TEXTURE_2D
        && _depthAttachment.second == tex) {
        return;
    }

    verifyBound();

    _depthAttachment.first = GL_TEXTURE_2D;
    _depthAttachment.second = tex;
    FF_GL_CALL(glFramebufferTexture2D,
        _bindTarget,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        tex,
        0);
}
void GLFramebuffer::attachDepthAttachmentAsRenderbuffer(GLuint const& tex) {
    if(_depthAttachment.first == GL_RENDERBUFFER
        && _depthAttachment.second == tex) {
        return;
    }

    verifyBound();

    _depthAttachment.first = GL_RENDERBUFFER;
    _depthAttachment.second = tex;
    FF_GL_CALL(glFramebufferRenderbuffer,
        _bindTarget,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        tex);
}
void GLFramebuffer::unbindDepthAttachment() {
    if(_depthAttachment.second == 0) {
        return;
    }

    verifyBound();

    _depthAttachment.second = 0;
    switch(_depthAttachment.first) {
    case GL_TEXTURE_2D:
        FF_GL_CALL(glFramebufferTexture2D,
            _bindTarget,
            GL_DEPTH_ATTACHMENT,
            GL_TEXTURE_2D,
            0,
            0);
        break;
    case GL_RENDERBUFFER:
        FF_GL_CALL(glFramebufferRenderbuffer,
            _bindTarget,
            GL_DEPTH_ATTACHMENT,
            GL_RENDERBUFFER,
            0);
        break;
    default:
        FF_CONSOLE_FATAL("Unknown attachment type.");
        break;
    }
}

void GLFramebuffer::setColorBlendState(int const& slot, std::optional<BlendState> const& blendState) {
    if((_graphicsDevice->getExtensions() & GLExtensions::ARB_DRAW_BUFFERS_BLEND) == 0) {
        // We don't have glBlendFuncSeparatei/glBlendEquationSeparatei.
        // aka, this is GLES 3.0. Use glBlendFuncSeparate/glBlendEquationSeparate,
        // assume slot = 0. This has already been vetted by the engine so this is safe.
        
        if(!blendState && _colorBlendEnable[0]) {
            FF_GL_CALL(glDisable, GL_BLEND);
            return;
        }
        if(blendState && !_colorBlendEnable[0]) {
            FF_GL_CALL(glEnable, GL_BLEND);

            if(_colorBlendStates[0] != blendState.value()) {
                if(_colorBlendStates[0].rgbBlendOperation != blendState->rgbBlendOperation
                    || _colorBlendStates[0].alphaBlendOperation != blendState->alphaBlendOperation) {
                    FF_GL_CALL(glBlendEquationSeparate,
                        convertBlendOperationToGLBlendOperation(blendState->rgbBlendOperation),
                        convertBlendOperationToGLBlendOperation(blendState->alphaBlendOperation));
                }
                if(_colorBlendStates[0].rgbSourceBlendFactor != blendState->rgbSourceBlendFactor
                    || _colorBlendStates[0].rgbDestBlendFactor != blendState->rgbDestBlendFactor
                    || _colorBlendStates[0].alphaSourceBlendFactor != blendState->alphaSourceBlendFactor
                    || _colorBlendStates[0].alphaDestBlendFactor != blendState->alphaDestBlendFactor) {
                    FF_GL_CALL(glBlendFuncSeparate,
                        convertBlendFactorToGLBlendFactor(blendState->rgbSourceBlendFactor),
                        convertBlendFactorToGLBlendFactor(blendState->rgbDestBlendFactor),
                        convertBlendFactorToGLBlendFactor(blendState->alphaSourceBlendFactor),
                        convertBlendFactorToGLBlendFactor(blendState->alphaDestBlendFactor));
                }
                _colorBlendStates[0] = blendState.value();
            }
        }
        return;
    }

    if(!blendState && _colorBlendEnable[slot]) {
        FF_GL_CALL(glDisablei, GL_BLEND, slot);
        return;
    }
    if(blendState && !_colorBlendEnable[slot]) {
        FF_GL_CALL(glEnablei, GL_BLEND, slot);

        if(_colorBlendStates[slot] != blendState.value()) {
            if(_colorBlendStates[slot].rgbBlendOperation != blendState->rgbBlendOperation
                || _colorBlendStates[slot].alphaBlendOperation != blendState->alphaBlendOperation) {
                FF_GL_CALL(glBlendEquationSeparatei,
                    slot,
                    convertBlendOperationToGLBlendOperation(blendState->rgbBlendOperation),
                    convertBlendOperationToGLBlendOperation(blendState->alphaBlendOperation));
            }
            if(_colorBlendStates[slot].rgbSourceBlendFactor != blendState->rgbSourceBlendFactor
                || _colorBlendStates[slot].rgbDestBlendFactor != blendState->rgbDestBlendFactor
                || _colorBlendStates[slot].alphaSourceBlendFactor != blendState->alphaSourceBlendFactor
                || _colorBlendStates[slot].alphaDestBlendFactor != blendState->alphaDestBlendFactor) {
                FF_GL_CALL(glBlendFuncSeparatei,
                    slot,
                    convertBlendFactorToGLBlendFactor(blendState->rgbSourceBlendFactor),
                    convertBlendFactorToGLBlendFactor(blendState->rgbDestBlendFactor),
                    convertBlendFactorToGLBlendFactor(blendState->alphaSourceBlendFactor),
                    convertBlendFactorToGLBlendFactor(blendState->alphaDestBlendFactor));
            }
            _colorBlendStates[slot] = blendState.value();
        }
    }
}

void GLFramebuffer::setDrawBuffers(std::array<GLenum, 4> const& drawBuffers) {
    FF_ASSERT(_graphicsDevice->getGlobalState()->getDrawFramebuffer() == _fbo,
        "Framebuffer must be bound as GL_DRAW_FRAMEBUFFER.");
    if(_drawBuffers != drawBuffers) {
        _drawBuffers = drawBuffers;
        FF_ASSERT(drawBuffers[0] != GL_NONE, "Draw buffers must have at least 1 buffer.");
        if(!_graphicsDevice->isGLES()) {
            FF_ASSERT(drawBuffers[0] != GL_BACK,
                      "In DesktopGL, GL_BACK is not valid for `glDrawBuffers`");
        }
        int n = 1;
        for (int i = 1; i < drawBuffers.size(); i++) {
            if (drawBuffers[i] != GL_NONE) {
                n++;
            }
            if (!_graphicsDevice->isGLES()) {
                FF_ASSERT(drawBuffers[i] != GL_BACK, "In DesktopGL, GL_BACK is not valid for `glDrawBuffers`");
            }
        }
        FF_GL_CALL(glDrawBuffers, n, _drawBuffers.data());
    }
}

void GLFramebuffer::assertStatus() {
    GLenum status = FF_GL_CALL(glCheckFramebufferStatus, _bindTarget);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        char const* reason;
        switch (status) {
        case GL_FRAMEBUFFER_UNDEFINED:
            reason = "GL_FRAMEBUFFER_UNDEFINED";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            reason = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            reason = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            reason = "GL_FRAMEBUFFER_UNSUPPORTED";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            reason = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            reason = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
            break;

        default:
            reason = "Unknown";
            break;
        }
        FF_CONSOLE_FATAL("Framebuffer incomplete: %s", reason);
    }
}

void GLFramebuffer::verifyBound() {
    FF_ASSERT((_bindTarget == GL_DRAW_FRAMEBUFFER ? _graphicsDevice->getGlobalState()->getDrawFramebuffer() : _graphicsDevice->getGlobalState()->getReadFramebuffer())
            == _fbo,
        "Framebuffer is not bound.");
}

}
