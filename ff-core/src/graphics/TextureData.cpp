/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/TextureData.hpp>

#include <ff/io/BinaryMemory.hpp>
#include <stb_image.h>

namespace ff {
    TextureData::TextureData(ff::IAssetBundle& assetBundle, const nlohmann::json& assetObject)
        :_width(-1),_height(-1),_format(TextureFormat::Invalid),_data(nullptr),_dataSize(0),_preMultipliedAlpha(false) {
        FF_ASSET_TYPE_CHECK(assetObject, "Texture");

        FF_ASSERT(!assetObject["path"].is_null(), "Missing `path` in asset `%s`.", assetObject["name"]);
        FF_ASSERT(!assetObject["pre-multiplied-alpha"].is_null(), "Missing `pre-multiplied-alpha` in asset `%s`.", assetObject["name"]);

        auto memory = assetBundle.load<BinaryMemory>(assetObject["path"]);
        _preMultipliedAlpha = assetObject["pre-multiplied-alpha"];
        initializeFromBinaryMemory(memory.get());
    }
    TextureData::TextureData(BinaryMemory& memory)
        :_width(-1),_height(-1),_format(TextureFormat::Invalid),_data(nullptr),_dataSize(0),_preMultipliedAlpha(false) {
        initializeFromBinaryMemory(&memory);
    }
    TextureData::TextureData(BinaryReader& reader)
        :_width(-1),_height(-1),_format(TextureFormat::Invalid),_data(nullptr),_dataSize(0),_preMultipliedAlpha(false) {
        BinaryMemory memory(reader);
        initializeFromBinaryMemory(&memory);
    }
    TextureData::TextureData(std::istream& stream)
        :_width(-1),_height(-1),_format(TextureFormat::Invalid),_data(nullptr),_dataSize(0),_preMultipliedAlpha(false) {
        BinaryMemory binaryMemory(stream);
        initializeFromBinaryMemory(&binaryMemory);
    }
    TextureData::TextureData(uint8_t* const& data, const size_t& dataSize, const int& width, const int& height, const TextureFormat& format, const bool& preMultipliedAlpha)
        :_dataSource(TextureDataSource::Raw),_width(width),_height(height),_format(format),_data(nullptr),_dataSize(dataSize),_preMultipliedAlpha(preMultipliedAlpha) {
        // @todo TextureData needs to be refactored after using it for a while.
        _data = malloc(_dataSize);
        memcpy(_data, data, _dataSize);

        // Very rarely do we _not_ need pre-multiplied alpha
        // In fact, we may even be able to get away with storing
        // only pre-multiplied alpha and storing non-pre-multiplied
        // if necessary instead of the other way around.
        if(_preMultipliedAlpha) {
            _preMultipliedAlphaMemory = std::make_shared<BinaryMemory>((uint8_t*)getData(), getDataSize());
        } else {
            if(!_preMultipliedAlphaMemory) {
                computePreMultipliedAlpha();
            }
        }
    }
    TextureData::~TextureData() {
        if(_data) {
            switch(_dataSource) {
            case TextureDataSource::Raw:
                delete[] (uint8_t*)_data;
                break;
            case TextureDataSource::STB:
                stbi_image_free(_data);
                break;
            default:
                break;
            }
        }
    }

    int TextureData::getWidth() const {
        return _width;
    }
    int TextureData::getHeight() const {
        return _height;
    }

    bool TextureData::isPreMultipliedAlpha() const {
        return _preMultipliedAlpha;
    }

    TextureFormat TextureData::getFormat() const {
        return _format;
    }

    void* TextureData::getData() const {
        return _data;
    }
    size_t TextureData::getDataSize() const {
        return _dataSize;
    }

    void* TextureData::getPreMultipliedAlphaData() const {
        if(_preMultipliedAlphaMemory == nullptr) {
            return nullptr;
        }
        return _preMultipliedAlphaMemory->data();
    }

    void TextureData::computePreMultipliedAlpha() {
        if(_preMultipliedAlphaMemory != nullptr) {
            return;
        }

        _preMultipliedAlphaMemory = std::make_shared<BinaryMemory>((uint8_t*)getData(), getDataSize());

        BufferFormatInfo_t formatInfo = BufferFormatInfo::get(getFormat());
        for(int i = 0; i < getWidth() * getHeight(); i++) {
            float a = ((float)_preMultipliedAlphaMemory->data()[formatInfo.componentCount - 1]) / 255.0f;

            // @todo Figure out a way to premultiply alpha without hardcoding the texture formats.

            // "- 2" because we skip the alpha (last) component
            for(int ii = 0; ii < formatInfo.componentCount - 2; ii++) {
                int idx = formatInfo.componentCount * i + ii;
                _preMultipliedAlphaMemory->data()[idx] = (uint8_t)(((float)_preMultipliedAlphaMemory->data()[idx]) * a);
            }
        }
    }

    void TextureData::initializeFromBinaryMemory(BinaryMemory* const& memory) {
        uint8_t* image = stbi_load_from_memory(memory->data(), (int)(memory->size() * sizeof(uint8_t)),
            &_width, &_height, 0, STBI_rgb_alpha);
        FF_ASSERT(image != nullptr, "Texture loaded incorrectly: %s.", stbi_failure_reason());

        _format = TextureFormat::RGBA8Unorm;
        BufferFormatInfo_t formatInfo = BufferFormatInfo::get(_format);

        _data = image;
        _dataSource = TextureDataSource::STB;
        _dataSize = _width * _height * formatInfo.size;

        // Very rarely do we _not_ need pre-multiplied alpha
        // In fact, we may even be able to get away with storing
        // only pre-multiplied alpha and storing non-pre-multiplied
        // if necessary instead of the other way around.
        if(!_preMultipliedAlphaMemory) {
            switch(_format) {
            case TextureFormat::RGBA8Unorm:
            case TextureFormat::RGBA32Uint:
                computePreMultipliedAlpha();
                break;
            default:
                break; // No alpha component
            }
        }
    }
}
