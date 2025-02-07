/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/io/StreamBinaryReader.hpp>

#include <ff/Console.hpp>

#include <iostream>

#include <glm/glm.hpp>

#include <stdint.h>

namespace ff {
    StreamBinaryReader::StreamBinaryReader(const std::shared_ptr<std::istream>& stream)
        :_stream(stream),_size(0) {
        FF_ASSERT((getStream()->flags() | std::istream::binary) != 0, "Stream must be opened in binary mode.");

        getStream()->seekg(0, std::ios_base::beg);
        std::streampos beg = getStream()->tellg();
        getStream()->seekg(0, std::ios_base::end);
        std::streampos end = getStream()->tellg();
        getStream()->seekg(0, std::ios_base::beg);
        std::streamsize size = end - beg;
        _size = (size_t)size;
    }
    StreamBinaryReader::~StreamBinaryReader() {
    }

    int StreamBinaryReader::getSize() const {
        return (int)_size;
    }
    int StreamBinaryReader::read(uint8_t* const& ptr, const int& count, const int& offset) {
        int bytesToRead = glm::min(count, getSize() - offset);

        getStream()->seekg(offset, std::ios_base::beg);
        getStream()->read(reinterpret_cast<char*>(ptr), bytesToRead);
        
        return bytesToRead;
    }
    const std::shared_ptr<std::istream>& StreamBinaryReader::getStream() const {
        return _stream;
    }
}
