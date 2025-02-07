/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLTextureImp.hpp>

#include <ff-graphics-opengl/GLBufferFormats.hpp>

#include <ff-graphics-opengl/GLGlobalState.hpp>
#include <ff-graphics-opengl/GLFramebufferManager.hpp>

#include <ff/Console.hpp>

namespace ff {

GLTextureImp::GLTextureImp(GLGraphicsDevice* const& graphicsDevice)
    :_graphicsDevice(graphicsDevice),
    _object(0),
    _textureType(GLTextureType::TEXTURE),
    _nonOwning(false) {
}
GLTextureImp::GLTextureImp(GLGraphicsDevice* const& graphicsDevice,
    GLuint const& texID)
    :_graphicsDevice(graphicsDevice),
    _object(texID),
    _textureType(GLTextureType::TEXTURE),
    _nonOwning(true) {
}
GLTextureImp::~GLTextureImp() {
    if(!_nonOwning) {
        destroy();
    }
}

bool GLTextureImp::isBackBuffer() const {
    return _object == 0;
}
GLTextureType GLTextureImp::getGLTextureType() const {
    return _textureType;
}
GLuint GLTextureImp::getObject() const {
    return _object;
}

void GLTextureImp::init(TextureType const& type,
    int const& width,
    int const& height,
    ColorBufferFormat const& format,
    TextureFlag_t const& flags,
    TextureUsage_t const& usage,
    std::optional<std::string> const& label) {
    FF_ASSERT(type == TextureType::COLOR, "Render target type must be COLOR to use ColorBufferFormat.");

    _height = height;

    if(_nonOwning) {
        FF_ASSERT((usage & (TextureUsage::GPU_SAMPLE
            | TextureUsage::CPU_READ
            | TextureUsage::CPU_WRITE)) == 0,
            "Non-owning render target cannot be sampled/have CPU access.");
        // @todo width/height check on non-owned texture? Not even sure if this is possible.
        //FF_ASSERT(width == _texture.width, "Width does not match non-owned texture width.");
        //FF_ASSERT(height == _texture.height, "Height does not match non-owned texture height.");

        return;
    }

    createObject(width,
        height,
        convertBufferFormatToGLSizedPixelFormat(format),
        flags,
        usage,
        label);
}
void GLTextureImp::init(TextureType const& type,
    int const& width,
    int const& height,
    DepthBufferFormat const& format,
    TextureFlag_t const& flags,
    TextureUsage_t const& usage,
    std::optional<std::string> const& label) {
    FF_ASSERT(type == TextureType::DEPTH, "Render target type must be DEPTH to use DepthBufferFormat.");

    _height = height;

    if(_nonOwning) {
        FF_ASSERT((usage & (TextureUsage::GPU_SAMPLE
            | TextureUsage::CPU_READ
            | TextureUsage::CPU_WRITE)) == 0,
            "Non-owning render target cannot be sampled/have CPU access.");
        // @todo width/height check on non-owned texture? Not even sure if this is possible.
        //FF_ASSERT(width == _texture.width, "Width does not match non-owned texture width.");
        //FF_ASSERT(height == _texture.height, "Height does not match non-owned texture height.");

        return;
    }

    createObject(width,
        height,
        convertBufferFormatToGLSizedPixelFormat(format),
        flags,
        usage,
        label);
}

void GLTextureImp::readPixels(void* dataOut,
    BufferFormatInfo_t const& formatInfo,
    int const& x,
    int const& y,
    int const& width,
    int const& height) {
    _graphicsDevice->getFramebufferManager()->prepareForCPUTransfer(this);
    auto formatType = convertGLSizedPixelFormatToGLPixelFormatAndType(_pixelFormat);
    FF_GL_CALL(glReadPixels, x, _height - y,
        width, height,
        formatType.first,
        formatType.second,
        dataOut);
}

void GLTextureImp::bufferImage(void* const& data,
    BufferFormatInfo_t const& formatInfo,
    int const& mipLevel,
    int const& subWidth,
    int const& subHeight,
    int const& xoffset,
    int const& yoffset) {
    FF_ASSERT(_textureType == GLTextureType::TEXTURE,
        "Texture type must be TEXTURE.");

    auto formatType = convertGLSizedPixelFormatToGLPixelFormatAndType(_pixelFormat);

    // We have to flip the texture upside-down because
    // the textures in FF are assumed to have (0, 0) as
    // top-left, but OpenGL has (0, 0) as bottom-left.
    std::vector<uint8_t> flippedData;
    flippedData.resize(subWidth * subHeight * formatInfo.size);
    uint8_t* const dataPtr = reinterpret_cast<uint8_t*>(data);
    for (int i = 0; i < subHeight; i++) {
        std::memcpy(&flippedData[i * subWidth * formatInfo.size], &dataPtr[(subHeight - 1 - i) * subWidth * formatInfo.size], subWidth * formatInfo.size);
    }

    bind();
    FF_GL_CALL(glTexSubImage2D,
        GL_TEXTURE_2D,
        mipLevel,
        xoffset,
        yoffset,
        subWidth,
        subHeight,
        formatType.first,
        formatType.second,
        flippedData.data());
}

void GLTextureImp::bind() {
    if(_object == 0) {
        return;
    }
    if(_textureType == GLTextureType::TEXTURE) {
        _graphicsDevice->getGlobalState()->bindTexture(GL_TEXTURE_2D,
            _object);
    } else {
        _graphicsDevice->getGlobalState()->bindRenderbuffer(_object);
    }
}
void GLTextureImp::attachToFramebufferAsColor(GLFramebuffer* const& fbo,
    int const& slot) {
    FF_ASSERT(_object != 0,
        "Texture is the backbuffer; backbuffer cannot be attached to a framebuffer directly (use `glDrawBuffers`)."); 

    if(_textureType == GLTextureType::TEXTURE) {
        fbo->attachColorAttachmentAsTexture2D(slot, _object);
    } else {
        fbo->attachColorAttachmentAsRenderBuffer(slot, _object);
    }
}
void GLTextureImp::attachToFramebufferAsDepth(GLFramebuffer* const& fbo) {
    FF_ASSERT(_object != 0,
        "Texture is the backbuffer; backbuffer cannot be attached to a framebuffer directly (use `glDrawBuffers`).");

    if(_textureType == GLTextureType::TEXTURE) {
        fbo->attachDepthAttachmentAsTexture2D(_object);
    } else {
        fbo->attachDepthAttachmentAsRenderbuffer(_object);
    }
}
void GLTextureImp::attachToSamplerSlot(int const& slot) {
    FF_ASSERT(_textureType == GLTextureType::TEXTURE,
        "Texture type must be TEXTURE.");

    _graphicsDevice->getGlobalState()->setActiveTexture(GL_TEXTURE0 + slot);
    bind();
}
void GLTextureImp::setTexParams(GLenum const& min,
    GLenum const& mag,
    GLenum const& sMode,
    GLenum const& tMode) {
    FF_ASSERT(_textureType == GLTextureType::TEXTURE,
        "Texture type must be TEXTURE.");

    bind();
    if(_minFilter == min
        && _magFilter == mag
        && _sAddressMode == sMode
        && _tAddressMode == tMode) {
        return;
    }
    bind();
    if(_minFilter != min) {
        _minFilter = min;
        FF_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
    }
    if(_magFilter != mag) {
        _magFilter = mag;
        FF_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
    }
    if(_sAddressMode != sMode) {
        _sAddressMode = sMode;
        FF_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sMode);
    }
    if(_tAddressMode != tMode) {
        _tAddressMode = tMode;
        FF_GL_CALL(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tMode);
    }
}
void GLTextureImp::genMipMaps() {
    FF_ASSERT(_textureType == GLTextureType::TEXTURE,
        "Texture type must be TEXTURE.");

    bind();
    // @todo Verify that the texture has mipmaps
    FF_GL_CALL(glGenerateMipmap, GL_TEXTURE_2D);
}

void GLTextureImp::destroy() {
    if(_object == 0) {
        return;
    }
    if(_textureType == GLTextureType::TEXTURE) {
        FF_GL_CALL(glDeleteTextures, 1, &_object);
    } else {
        FF_GL_CALL(glDeleteRenderbuffers, 1, &_object);
    }
    _object = 0;
}
void GLTextureImp::createObject(int const& width,
    int const& height,
    GLenum const& pixelFormat,
    TextureFlag_t const& flags,
    TextureUsage_t const& usage,
    std::optional<std::string> const& label) {
    _pixelFormat = pixelFormat;

    // Look at usage, see if we should use
    // a texture or renderbuffer.
    
    // Renderbuffers can be used unless `usage` has
    // any of the following:
    //  - CPU_WRITE
    //  - GPU_SAMPLE
    //  - GPU_LOAD
    //
    //  And must not have any of the following `flags`
    //  @todo Mipmap

    if((usage & (TextureUsage::CPU_WRITE
        | TextureUsage::GPU_SAMPLE
        | TextureUsage::GPU_LOAD)) > 0) {
        createAsTexture(width,
            height,
            flags,
            usage,
            label);
    } else {
        createAsRenderbuffer(width,
            height,
            flags,
            usage,
            label);
    }
}
void GLTextureImp::createAsTexture(int const& width,
    int const& height,
    TextureFlag_t const& flags,
    TextureUsage_t const& usage,
    std::optional<std::string> const& label) {
    FF_GL_CALL(glGenTextures, 1, &_object);
    _textureType = GLTextureType::TEXTURE;
    FF_ASSERT(_object != 0, "Error in creating GL texture.");

    auto formatType = convertGLSizedPixelFormatToGLPixelFormatAndType(_pixelFormat);

    // @todo Verify width/height are within device limitations?
    bind();

    if (label != std::nullopt) {
        _graphicsDevice->glObjectLabel(GL_TEXTURE,
            _object,
            label.value().size(),
            label.value().c_str());
    }

    if((_graphicsDevice->getExtensions() & GLExtensions::ARB_TEXTURE_STORAGE) > 0) {
        FF_GL_CALL(glTexStorage2D,
            GL_TEXTURE_2D,
            1, // @todo Mipmaps
            _pixelFormat,
            width,
            height);
    } else {
        FF_GL_CALL(glTexParameteri, GL_TEXTURE_2D,
            GL_TEXTURE_BASE_LEVEL, 0);
        FF_GL_CALL(glTexParameteri, GL_TEXTURE_2D,
            GL_TEXTURE_MAX_LEVEL, 0); // @todo Mipmaps
        FF_GL_CALL(glTexImage2D,
            GL_TEXTURE_2D,
            0, // @todo Mipmaps
            _pixelFormat,
            width,
            height,
            0, // Must be 0 according to docs
            formatType.first, // Not strictly needed b/c data is nullptr
            formatType.second, // Not strictly needed b/c data is nullptr
            nullptr);
    }
}
void GLTextureImp::createAsRenderbuffer(int const& width,
    int const& height,
    TextureFlag_t const& flags,
    TextureUsage_t const& usage,
    std::optional<std::string> const& label) {
    FF_GL_CALL(glGenRenderbuffers, 1, &_object);
    _textureType = GLTextureType::RENDERBUFFER;
    FF_ASSERT(_object != 0, "Error in creating GL renderbuffer.");

    bind();

    if (label != std::nullopt) {
        _graphicsDevice->glObjectLabel(GL_RENDERBUFFER,
            _object,
            label.value().size(),
            label.value().c_str());
    }

    FF_GL_CALL(glRenderbufferStorage,
        GL_RENDERBUFFER,
        _pixelFormat,
        width,
        height);
}

}
