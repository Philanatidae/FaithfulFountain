/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_TEXTURE_DATA_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_TEXTURE_DATA_HPP

#include <ff/assets/IAssetBundle.hpp>

#include <ff/io/BinaryMemory.hpp>

#include <ff/graphics/BufferFormats.hpp>

namespace ff {
    enum class TextureDataSource {
        Raw,
        STB
    };

    class TextureData {
    public:
        TextureData(ff::IAssetBundle& assetBundle, const nlohmann::json& assetObject);
        TextureData(BinaryMemory& memory);
        TextureData(BinaryReader& reader);
        TextureData(std::istream& stream);
        TextureData(uint8_t* const& data,
            const size_t& dataSize,
            const int& width,
            const int& height,
            const TextureFormat& format,
            const bool& preMultipliedAlpha);
        virtual ~TextureData();

        int getWidth() const;
        int getHeight() const;

        bool isPreMultipliedAlpha() const;

        TextureFormat getFormat() const;

        void* getData() const;
        size_t getDataSize() const;

        void* getPreMultipliedAlphaData() const;

    private:
        TextureDataSource _dataSource;
        int _width;
        int _height;
        TextureFormat _format;
        bool _preMultipliedAlpha;
        void* _data;
        size_t _dataSize;
        std::shared_ptr<BinaryMemory> _preMultipliedAlphaMemory;

        void initializeFromBinaryMemory(BinaryMemory* const& memory);
        void computePreMultipliedAlpha();
    };
}

#endif
