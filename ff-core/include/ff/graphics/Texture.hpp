/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_TEXTURE_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_TEXTURE_HPP

#include <ff/assets/IAssetBundle.hpp>
#include <stdint.h>
#include <cstring>

#include <ff/graphics/BufferFormats.hpp>
#include <ff/graphics/TextureData.hpp>
#include <ff/graphics/TextureTypes.hpp>

#include <optional>

namespace ff {

class TextureImp {
public:
    virtual ~TextureImp() = default;
 
    virtual void init(TextureType const& type,
        int const& width,
        int const& height,
        ColorBufferFormat const& format,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        std::optional<std::string> const& label) = 0;
    virtual void init(TextureType const& type,
        int const& width,
        int const& height,
        DepthBufferFormat const& format,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        std::optional<std::string> const& label) = 0;

    virtual void readPixels(void* dataOut,
        BufferFormatInfo_t const& formatInfo,
        int const& x,
        int const& y,
        int const& width,
        int const& height) = 0;

    virtual void bufferImage(void* const& data,
        BufferFormatInfo_t const& formatInfo,
        int const& mipLevel,
        int const& subWidth,
        int const& subHeight,
        int const& xoffset,
        int const& yoffset) = 0;
};

class NullTextureImp : public TextureImp {
public:
    virtual ~NullTextureImp() = default;

    void init(TextureType const& type,
        int const& width,
        int const& height,
        ColorBufferFormat const& format,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        std::optional<std::string> const& label) override {
    }
    void init(TextureType const& type,
        int const& width,
        int const& height,
        DepthBufferFormat const& format,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        std::optional<std::string> const& label) override {
    }

    virtual void readPixels(void* dataOut,
        BufferFormatInfo_t const& formatInfo,
        int const& x,
        int const& y,
        int const& width,
        int const& height) override {
        std::memset(dataOut,
            0,
            formatInfo.size * width * height);
    }

    void bufferImage(void* const& data,
        BufferFormatInfo_t const& formatInfo,
        int const& mipLevel,
        int const& subWidth,
        int const& subHeight,
        int const& xoffset,
        int const& yoffset) override {
    }
};

class ITexture {
public:
    virtual ~ITexture() = default;

    virtual TextureImp* getImp() const = 0;
    virtual TextureImp* getImp() = 0;
};

template<typename T>
class Texture : public ITexture {
public:
    Texture(TextureImp* imp,
        TextureType const& type,
        T const& format,
        int const& width,
        int const& height,
        TextureFlag_t const& flags,
        TextureUsage_t const& usage,
        std::optional<std::string> const& label);
    virtual ~Texture() = default;

    using bufferInfoType = T;

    TextureImp* getImp() const override;
    TextureImp* getImp() override;

    T getFormat() const;

    int getWidth() const;
    int getHeight() const;
    float getAspectRatio() const;

    TextureFlag_t getFlags() const;
    TextureUsage_t getUsage() const;

    std::optional<std::string> getLabel() const;

    std::shared_ptr<BinaryMemory> readPixels(int const& x,
        int const& y,
        int const& width,
        int const& height) const;

    void bufferImage(ResourceHandle<TextureData> const& textureData,
        bool const& forceNonPremultipliedAlpha = false);
    void bufferImage(void* const& data,
        int const& mipLevel = 0);
    void bufferImage(void* const& data,
        int const& mipLevel,
        int const& subWidth,
        int const& subHeight);
    void bufferImage(void* const& data,
        int const& mipLevel,
        int const& subWidth,
        int const& subHeight,
        int const& xoffset,
        int const& yoffset);

private:
    TextureImp* _imp;

    TextureType _type;
    T const _format;
    int const _width;
    int const _height;
    TextureFlag_t const _flags;
    TextureUsage_t const _usage;
    std::optional<std::string> _label;
};

using ColorTexture = Texture<ColorBufferFormat>;
using DepthTexture = Texture<DepthBufferFormat>;

template<>
struct DefaultAssetLoader<ColorTexture> {
    static ColorTexture* load(IAssetBundle& assetBundle, const nlohmann::json& assetObject);
};

template<typename T>
Texture<T>::Texture(TextureImp* imp,
    TextureType const& type,
    T const& format,
    int const& width,
    int const& height,
    TextureFlag_t const& flags,
    TextureUsage_t const& usage,
    std::optional<std::string> const& label)
    :_imp(imp),
    _type(type),
    _width(width),
    _height(height),
    _format(format),
    _flags(flags),
    _usage(usage),
    _label(label) {
    FF_ASSERT(_width > 0, "Width must be a positive non-zero integer (is `%s`).", _width);
    FF_ASSERT(_height > 0, "Height must be a positive non-zero integer (is `%s`).", _height);

    _imp->init(type,
        width,
        height,
        format,
        flags,
        usage,
        label);
}

template<typename T>
TextureImp* Texture<T>::getImp() const {
    return _imp;
}
template<typename T>
TextureImp* Texture<T>::getImp() {
    return _imp;
}

template<typename T>
T Texture<T>::getFormat() const {
    return _format;
}

template<typename T>
int Texture<T>::getWidth() const {
    return _width;
}
template<typename T>
int Texture<T>::getHeight() const {
    return _height;
}
template<typename T>
float Texture<T>::getAspectRatio() const {
    return (float)_width / _height;
}

template<typename T>
TextureFlag_t Texture<T>::getFlags() const {
    return _flags;
}
template<typename T>
TextureUsage_t Texture<T>::getUsage() const {
    return _usage;
}

template<typename T>
std::optional<std::string> Texture<T>::getLabel() const {
    return _label;
}

template<typename T>
std::shared_ptr<BinaryMemory> Texture<T>::readPixels(int const& x,
    int const& y,
    int const& width,
    int const& height) const {
    FF_ASSERT((_usage & TextureUsage::CPU_READ) > 0, "Texture must have CPU_READ usage.");

    FF_ASSERT(x >= 0, "X must be positive.");
    FF_ASSERT(x < getWidth(), "X out-of-bounds.");
    FF_ASSERT(y >= 0, "Y must be positive.");
    FF_ASSERT(y < getHeight(), "Y out-of-bounds.");
    FF_ASSERT(width > 0, "Width must be positive.");
    FF_ASSERT(x + width <= getWidth(), "Width out-of-bounds.");
    FF_ASSERT(height > 0, "Height must be positive.");
    FF_ASSERT(y + height <= getHeight(), "Width out-of-bounds.");

    auto const formatInfo = BufferFormatInfo::get(getFormat());
    auto mem = std::make_shared<BinaryMemory>(formatInfo.size * width * height);
    _imp->readPixels(mem->data(),
        formatInfo,
        x, y,
        width, height);
    return mem;
}

template<typename T>
void Texture<T>::bufferImage(ResourceHandle<TextureData> const& textureData,
        bool const& forceNonPremultipliedAlpha) {
    FF_ASSERT(BufferFormatInfo::matches(textureData->getFormat(), _format),
        "Texture data format does not match texture format.");
    if(!textureData->isPreMultipliedAlpha()
        || forceNonPremultipliedAlpha) {
        bufferImage(textureData->getData());
    } else {
        bufferImage(textureData->getPreMultipliedAlphaData());
    }
}
template<typename T>
void Texture<T>::bufferImage(void* const& data,
    int const& mipLevel) {
    bufferImage(data,
        mipLevel,
        getWidth(),
        getHeight(),
        0,
        0);
}
template<typename T>
void Texture<T>::bufferImage(void* const& data,
    int const& mipLevel,
    int const& subWidth,
    int const& subHeight) {
    bufferImage(data,
        mipLevel,
        getWidth(),
        getHeight());
}
template<typename T>
void Texture<T>::bufferImage(void* const& data,
    int const& mipLevel,
    int const& subWidth,
    int const& subHeight,
    int const& xoffset,
    int const& yoffset) {
    FF_ASSERT((_usage & TextureUsage::CPU_WRITE) > 0, "Texture must have CPU_WRITE usage.");

    FF_ASSERT(xoffset >= 0, "X must be positive.");
    FF_ASSERT(xoffset < getWidth(), "X out-of-bounds.");
    FF_ASSERT(yoffset >= 0, "Y must be positive.");
    FF_ASSERT(yoffset < getHeight(), "Y out-of-bounds.");
    FF_ASSERT(subWidth > 0, "Width must be positive.");
    FF_ASSERT(xoffset + subWidth <= getWidth(), "Width out-of-bounds.");
    FF_ASSERT(subHeight > 0, "Height must be positive.");
    FF_ASSERT(yoffset + subHeight <= getHeight(), "Width out-of-bounds.");

    _imp->bufferImage(data,
        BufferFormatInfo::get(_format),
        mipLevel,
        subWidth,
        subHeight,
        xoffset,
        yoffset);
}

}

#endif

