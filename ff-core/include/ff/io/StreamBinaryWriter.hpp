/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_IO_STREAM_BINARY_WRITER_HPP
#define _FAITHFUL_FOUNTAIN_IO_STREAM_BINARY_WRITER_HPP

#include <ff/io/BinaryWriter.hpp>
#include <iosfwd>
#include <memory>

namespace ff {
    class StreamBinaryWriter : public BinaryWriter {
    public:
        StreamBinaryWriter(const std::shared_ptr<std::ostream>& stream);
        virtual ~StreamBinaryWriter();

        int write(uint8_t* const& src, const int& count, const int& offset = 0) override;

        const std::shared_ptr<std::ostream>& getStream() const;
    
    private:
        std::shared_ptr<std::ostream> _stream;
    };
}

#endif
