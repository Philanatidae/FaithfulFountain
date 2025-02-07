/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLGlobalState.hpp>

#include <ff-graphics-opengl/GLGraphicsDevice.hpp>

namespace ff {
    
GLGlobalState::GLGlobalState(GLGraphicsDevice* const& graphicsDevice)
    :_graphicsDevice(graphicsDevice),
    _clearColor(Color::transparentBlack()),
    _clearDepth(1.0f),
    _cullFaceEnabled(false),
    _scissorEnabled(false),
    _depthEnabled(false),
    _blendEnabled(false),
    _frontFace(GL_CCW),
    _cullFace(GL_BACK),
    _depthFunc(0),
    _depthWriteEnabled(false),
    _activeTexture(GL_TEXTURE0),
    _program(0),
    _drawFBO(0),
    _readFBO(0),
    _boundFBO(0),
    _readBuffer(GL_BACK),
    _boundVAO(0),
    _boundRenderbuffer(0) {
}

void GLGlobalState::init() {
    // Register textures names available in OpenGL ES 3.0
    // Additional types can be added if we move to
    // 3.1 or 3.2.
    std::unordered_map<GLenum, GLuint> defaultBoundTextures;
    defaultBoundTextures.emplace(GL_TEXTURE_2D, 0);
    defaultBoundTextures.emplace(GL_TEXTURE_3D, 0);
    defaultBoundTextures.emplace(GL_TEXTURE_2D_ARRAY, 0);
    defaultBoundTextures.emplace(GL_TEXTURE_CUBE_MAP, 0);
    for(int i = 0; i < _graphicsDevice->getMaxTextureUnits(); i++) {
        _boundTextures.emplace(GL_TEXTURE0 + i, defaultBoundTextures);
    }

    _boundBuffers[GL_ARRAY_BUFFER] = 0;
    _boundBuffers[GL_ELEMENT_ARRAY_BUFFER] = 0;
    _boundBuffers[GL_UNIFORM_BUFFER] = 0;
}

void GLGlobalState::setClearColor(Color const& color) {
    if(_clearColor == color) {
        return;
    }
    _clearColor = color;
    FF_GL_CALL(glClearColor,
        color.r,
        color.g,
        color.b,
        color.a);
}
void GLGlobalState::setClearDepth(float const& depth) {
    if(_clearDepth == depth) { // @todo Epsilon
        return;
    }
    _clearDepth = depth;
#if defined(FF_GL_ES)
    FF_GL_CALL(glClearDepthf, depth);
#else
    FF_GL_CALL(glClearDepth, depth);
#endif
}

void GLGlobalState::setCullFaceEnabled(bool const& enabled) {
    if(_cullFaceEnabled == enabled) {
        return;
    }
    _cullFaceEnabled = enabled;
    if(enabled) {
        FF_GL_CALL(glEnable, GL_CULL_FACE);
    } else {
        FF_GL_CALL(glDisable, GL_CULL_FACE);
    }
}
void GLGlobalState::setScissorEnabled(bool const& enabled) {
    if(_scissorEnabled == enabled) {
        return;
    }
    _scissorEnabled = enabled;
    if(enabled) {
        FF_GL_CALL(glEnable, GL_SCISSOR_TEST);
    } else {
        FF_GL_CALL(glDisable, GL_SCISSOR_TEST);
    }
}
void GLGlobalState::setDepthEnabled(bool const& enabled) {
    if(_depthEnabled == enabled) {
        return;
    }
    _depthEnabled = enabled;
    if(enabled) {
        FF_GL_CALL(glEnable, GL_DEPTH_TEST);
    } else {
        FF_GL_CALL(glDisable, GL_DEPTH_TEST);
    }
}
void GLGlobalState::setBlendEnabled(bool const& enabled) {
    if(_blendEnabled == enabled) {
        return;
    }
    _blendEnabled = enabled;
    if(enabled) {
        FF_GL_CALL(glEnable, GL_BLEND);
    } else {
        FF_GL_CALL(glDisable, GL_BLEND);
    }
}

void GLGlobalState::setFrontFace(GLenum const& frontFace) {
    if(_frontFace == frontFace) {
        return;
    }
    _frontFace = frontFace;
    FF_GL_CALL(glFrontFace, frontFace);
}
void GLGlobalState::setCullFace(GLenum const& cullFace) {
    if(_cullFace == cullFace) {
        return;
    }
    _cullFace = cullFace;
    FF_GL_CALL(glCullFace, cullFace);
}

void GLGlobalState::setDepthFunction(GLenum const& func) {
    if(_depthFunc == func) {
        return;
    }
    _depthFunc = func;
    FF_GL_CALL(glDepthFunc, func);
}
void GLGlobalState::setDepthWriteEnabled(bool const& enabled) {
    if(_depthWriteEnabled == enabled) {
        return;
    }
    _depthFunc = enabled;
    FF_GL_CALL(glDepthMask, enabled);
}

void GLGlobalState::setActiveTexture(GLenum const& activeTexture) {
    if(_activeTexture == activeTexture) {
        return;
    }
    _activeTexture = activeTexture;
    FF_GL_CALL(glActiveTexture, activeTexture);
}
void GLGlobalState::bindTexture(GLenum const& type, GLuint const& tex) {
    FF_ASSERT(_boundTextures[_activeTexture].find(type) != _boundTextures[_activeTexture].end(), "Texture type `%s` is not known (this could be a type introduced >= OpenGL ES 3.1).");

    if(_boundTextures[_activeTexture][type] == tex) {
        return;
    }
    _boundTextures[_activeTexture][type] = tex;
    FF_GL_CALL(glBindTexture, type, tex);
}

void GLGlobalState::bindBuffer(GLenum const& type, GLuint const& buff) {
    // From Kronos documentation:
    //
    // Indexed rendering, as defined above, requires an array
    // of indices; all vertex attributes will use the same
    // index from this index array. The index array is provided
    // by a Buffer Object bound to the GL_ELEMENT_ARRAY_BUFFER
    // binding point. When a buffer is bound to
    // GL_ELEMENT_ARRAY_BUFFER, all drawing commands of the form
    // gl*Draw*Elements* will use indexes from that buffer.
    // Indices can be unsigned bytes, unsigned shorts, or unsigned ints.
    //
    // Source: https://www.khronos.org/opengl/wiki/Vertex_Specification
    //
    // Therefore, index buffers are always part of the VAO state, not
    // global state.
    //
    // HOWEVER, because of the way FF is designed, we don't consider
    // index buffers to be "bound" to vertex descriptors (VAOs in GL).
    // So we can just bind away and not care (may not be as efficient,
    // but we don't sacrifice Metal/Vulkan/DX performance for GL performance).
    if(type == GL_ELEMENT_ARRAY_BUFFER) {
        if(_boundIndexBuffers[_boundVAO] == buff) {
            return;
        }
        _boundIndexBuffers[_boundVAO] = buff;
        FF_GL_CALL(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, buff);
        return;
    }

    FF_ASSERT(_boundBuffers.find(type) != _boundBuffers.end(), "Buffer type `%s` is not known (this could be a type introduced >= OpenGL ES 3.1).");
    if(_boundBuffers[type] == buff) {
        return;
    }
    _boundBuffers[type] = buff;
    FF_GL_CALL(glBindBuffer, type, buff);
}

void GLGlobalState::bindProgram(GLuint const& program) {
    if(_program == program) {
        return;
    }
    _program = program;
    FF_GL_CALL(glUseProgram, program);
}

void GLGlobalState::bindDrawFramebuffer(GLuint const& fbo) {
    if(_drawFBO == fbo) {
        return;
    }
    _boundFBO = _drawFBO = fbo;
    FF_GL_CALL(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, fbo);
}
void GLGlobalState::bindReadFramebuffer(GLuint const& fbo) {
    if(_readFBO == fbo) {
        return;
    }
    _boundFBO = _readFBO = fbo;
    FF_GL_CALL(glBindFramebuffer, GL_READ_FRAMEBUFFER, fbo);
}
GLuint GLGlobalState::getDrawFramebuffer() const {
    return _drawFBO;
}
GLuint GLGlobalState::getReadFramebuffer() const {
    return _readFBO;
}
GLuint GLGlobalState::getLastBoundFramebuffer() const {
    return _boundFBO;
}

void GLGlobalState::setReadBuffer(GLenum const& src) {
    if(_readBuffer == src) {
        return;
    }
    _readBuffer = src;
    FF_GL_CALL(glReadBuffer, src);
}

//void GLGlobalState::setDrawBuffers(GLenum const& dst1,
//    GLenum const& dst2,
//    GLenum const& dst3,
//    GLenum const& dst4) {
//    if(_drawBuffers[0] == dst1
//        && _drawBuffers[1] == dst2
//        && _drawBuffers[2] == dst3
//        && _drawBuffers[3] == dst4) {
//        return;
//    }
//    _drawBuffers[0] = dst1;
//    _drawBuffers[1] = dst2;
//    _drawBuffers[2] = dst3;
//    _drawBuffers[3] = dst4;
//    FF_GL_CALL(glDrawBuffers, 4, _drawBuffers.data());
//}

void GLGlobalState::bindVAO(GLuint const& vao) {
    if(_boundVAO == vao) {
        return;
    }
    _boundVAO = vao;
    FF_GL_CALL(glBindVertexArray, vao);

    if(_boundBuffers.find(vao) == _boundBuffers.end()) {
        _boundBuffers.emplace(vao, 0);
    }
}

void GLGlobalState::bindRenderbuffer(GLuint const& rb) {
    if(_boundRenderbuffer == rb) {
        return;
    }
    _boundRenderbuffer = rb;
    FF_GL_CALL(glBindRenderbuffer,
        GL_RENDERBUFFER,
        rb);
}

}
