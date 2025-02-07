/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_FRAMEBUFFER_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_FRAMEBUFFER_HPP

#include <ff-graphics-opengl/gl.hpp>
#include <ff-graphics-opengl/GLGraphicsDevice.hpp>

#include <ff/graphics/BlendState.hpp>
#include <optional>

#include <array>

namespace ff {

class GLGraphicsDevice;

class GLFramebuffer final {
public:
    GLFramebuffer(GLGraphicsDevice* const& graphicsDevice, GLuint const& fbo);
    GLFramebuffer(GLGraphicsDevice* const& graphicsDevice);
    ~GLFramebuffer();

    std::array<GLenum, 4> getDrawBuffers() const;

    void bindAsDraw();
    void bindAsRead();

    void attachColorAttachmentAsTexture2D(int const& slot,
        GLuint const& tex);
    void attachColorAttachmentAsRenderBuffer(int const& slot,
        GLuint const& rb);
    void unbindColorAttachment(int const& slot);

    void attachDepthAttachmentAsTexture2D(GLuint const& tex);
    void attachDepthAttachmentAsRenderbuffer(GLuint const& tex);
    void unbindDepthAttachment();

    void setColorBlendState(int const& slot, std::optional<BlendState> const& blendState);

    void setDrawBuffers(std::array<GLenum, 4> const& drawBuffers);

    void assertStatus();

private:
    void verifyBound();

    GLGraphicsDevice* const _graphicsDevice;
    GLuint _fbo;
    bool _owned;
    GLenum _bindTarget;

    std::array<std::pair<GLenum, GLuint>, 4> _colorAttachments;
    std::array<BlendState, 4> _colorBlendStates;
    std::array<bool, 4> _colorBlendEnable;
    std::pair<GLenum, GLuint> _depthAttachment;

    std::array<GLenum, 4> _drawBuffers;
};

}

#endif

