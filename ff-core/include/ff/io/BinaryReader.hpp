/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_IO_BINARY_READER_HPP
#define _FAITHFUL_FOUNTAIN_IO_BINARY_READER_HPP

#include <stdint.h>

namespace ff {
    class BinaryReader {
    public:
        BinaryReader();
        virtual ~BinaryReader();

        virtual int getSize() const = 0;
        virtual int read(uint8_t* const& ptr, const int& count, const int& offset = 0) = 0;
    };
}

#endif
