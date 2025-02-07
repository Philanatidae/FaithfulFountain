/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_GLOBAL_STATE_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_GLOBAL_STATE_HPP

#include <ff-graphics-opengl/gl.hpp>
#include <optional>

#include <ff/graphics/Color.hpp>

#include <unordered_map>
#include <vector>
#include <array>

namespace ff {

class GLGraphicsDevice;

class GLGlobalState final {
public:
    GLGlobalState(GLGraphicsDevice* const& graphicsDevice);
    ~GLGlobalState() = default;

    void init();

    void setClearColor(Color const& color);
    void setClearDepth(float const& depth);

    void setCullFaceEnabled(bool const& enabled);
    void setScissorEnabled(bool const& enabled);
    void setDepthEnabled(bool const& enabled);
    void setBlendEnabled(bool const& enabled);

    void setFrontFace(GLenum const& frontFace);
    void setCullFace(GLenum const& cullFace);

    void setDepthFunction(GLenum const& func);
    void setDepthWriteEnabled(bool const& enabled);

    void setActiveTexture(GLenum const& activeTexture);
    void bindTexture(GLenum const& type, GLuint const& tex);

    void bindBuffer(GLenum const& type, GLuint const& buff);

    void bindProgram(GLuint const& program);

    void bindDrawFramebuffer(GLuint const& fbo);
    void bindReadFramebuffer(GLuint const& fbo);
    GLuint getDrawFramebuffer() const;
    GLuint getReadFramebuffer() const;
    GLuint getLastBoundFramebuffer() const;

    void setReadBuffer(GLenum const& src);

    void bindVAO(GLuint const& vao);

    void bindRenderbuffer(GLuint const& rb);

private:
    GLGraphicsDevice* _graphicsDevice;

    ff::Color _clearColor;
    float _clearDepth;

    bool _cullFaceEnabled;
    bool _scissorEnabled;
    bool _depthEnabled;
    bool _blendEnabled;

    GLenum _frontFace;
    GLenum _cullFace;

    GLenum _depthFunc;
    bool _depthWriteEnabled;

    GLenum _activeTexture;
    std::unordered_map<GLenum, std::unordered_map<GLenum, GLuint>> _boundTextures;

    std::unordered_map<GLenum, GLuint> _boundBuffers;
    std::unordered_map<GLuint, GLuint> _boundIndexBuffers;
    std::unordered_map<GLenum, GLenum> _bufferTargetBindingMap;

    GLuint _program;

    GLuint _drawFBO;
    GLuint _readFBO;
    GLuint _boundFBO;

    GLuint _readBuffer;

    GLuint _boundVAO;

    GLuint _boundRenderbuffer;
};

}

#endif
