/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_IO_STREAM_BINARY_READER_HPP
#define _FAITHFUL_FOUNTAIN_IO_STREAM_BINARY_READER_HPP

#include <ff/io/BinaryReader.hpp>
#include <iosfwd>
#include <memory>

namespace ff {
    class StreamBinaryReader : public BinaryReader {
    public:
        StreamBinaryReader(const std::shared_ptr<std::istream>& stream);
        virtual ~StreamBinaryReader();

        int getSize() const;
        int read(uint8_t* const& ptr, const int& count, const int& offset = 0);

        const std::shared_ptr<std::istream>& getStream() const;

    private:
        std::shared_ptr<std::istream> _stream;
        size_t _size;
    };
}

#endif
