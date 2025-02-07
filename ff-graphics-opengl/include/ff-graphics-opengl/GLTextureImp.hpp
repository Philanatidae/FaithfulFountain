/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_TEXTURE_IMP_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_TEXTURE_IMP_HPP

#include <ff-graphics-opengl/gl.hpp>
#include <ff-graphics-opengl/GLGraphicsDevice.hpp>
#include <ff-graphics-opengl/GLFramebuffer.hpp>

#include <ff/graphics/Texture.hpp>

namespace ff {

enum class GLTextureType {
    TEXTURE,
    RENDERBUFFER
};

class GLTextureImp : public TextureImp {
public:
    GLTextureImp(GLGraphicsDevice* const& graphicsDevice); // owned
    GLTextureImp(GLGraphicsDevice* const& graphicsDevice,
        GLuint const& texID); // non-owned
    ~GLTextureImp();

    bool isBackBuffer() const;
    GLTextureType getGLTextureType() const;
    GLuint getObject() const;

    void init(TextureType const& type,
        int const& width,
        int const& height,
        ColorBufferFormat const& format,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        std::optional<std::string> const& label) override;
    void init(TextureType const& type,
        int const& width,
        int const& height,
        DepthBufferFormat const& format,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        std::optional<std::string> const& label) override;

    void readPixels(void* dataOut,
        BufferFormatInfo_t const& formatInfo,
        int const& x,
        int const& y,
        int const& width,
        int const& height) override;

    void bufferImage(void* const& data,
        BufferFormatInfo_t const& formatInfo,
        int const& mipLevel,
        int const& subWidth,
        int const& subHeight,
        int const& xoffset,
        int const& yoffset) override;

    void bind();
    void attachToFramebufferAsColor(GLFramebuffer* const& fbo,
        int const& slot);
    void attachToFramebufferAsDepth(GLFramebuffer* const& fbo);
    void attachToSamplerSlot(int const& slot);
    void setTexParams(GLenum const& min,
        GLenum const& mag,
        GLenum const& sMode,
        GLenum const& tMode);
    void genMipMaps();

private:
    void destroy();
    void createObject(int const& width,
        int const& height,
        GLenum const& pixelFormat,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        std::optional<std::string> const& label);
    void createAsTexture(int const& width,
        int const& height,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        std::optional<std::string> const& label);
    void createAsRenderbuffer(int const& width,
        int const& height,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        std::optional<std::string> const& label);

    GLGraphicsDevice* const _graphicsDevice;

    GLuint _object;
    GLTextureType _textureType;
    bool _nonOwning;

    float _height;

    GLenum _pixelFormat;

    GLenum _minFilter;
    GLenum _magFilter;
    GLenum _sAddressMode;
    GLenum _tAddressMode;
};

}

#endif
