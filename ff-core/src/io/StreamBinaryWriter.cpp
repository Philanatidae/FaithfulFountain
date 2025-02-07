/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/io/StreamBinaryWriter.hpp>

#include <ff/Console.hpp>

#include <iostream>

namespace ff {
    StreamBinaryWriter::StreamBinaryWriter(const std::shared_ptr<std::ostream>& stream)
        :_stream(stream) {
        FF_ASSERT((getStream()->flags() | std::istream::binary) != 0, "Stream must be opened in binary mode.");
    }
    StreamBinaryWriter::~StreamBinaryWriter() {
    }

    int StreamBinaryWriter::write(uint8_t* const& src, const int& count, const int& offset) {
        getStream()->write((char*)(src + offset), count);
        return count;
    }

    const std::shared_ptr<std::ostream>& StreamBinaryWriter::getStream() const {
        return _stream;
    }
}
