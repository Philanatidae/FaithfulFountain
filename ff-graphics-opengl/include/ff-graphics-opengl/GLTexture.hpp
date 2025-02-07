/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_SHADER_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_SHADER_HPP

#include <ff-graphics-opengl/gl.hpp>

#include <ff/graphics/Texture.hpp>
#include <ff/graphics/TextureData.hpp>

#include <ff-graphics-opengl/GLGraphicsDevice.hpp>
#include <ff-graphics-opengl/GLBufferFormats.hpp>

namespace ff {

class GLTexture final : public ColorTexture {
public:
    GLTexture(GLGraphicsDevice* const& graphicsDevice,
        const ResourceHandle<TextureData>& textureData,
        TextureFlag_t const& flags = 0);
    GLTexture(GLGraphicsDevice* const& graphicsDevice,
        int const& width,
        int const& height,
        GLenum const& format,
        TextureFlag_t const& flags = 0);
    ~GLTexture();

    int getWidth() const override;
    int getHeight() const override;

    GLuint getGLTexture() const;

private:
    GLuint _texture;
};

}

#endif
