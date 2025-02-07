/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_IO_BINARY_WRITER_HPP
#define _FAITHFUL_FOUNTAIN_IO_BINARY_WRITER_HPP

#include <stdint.h>

namespace ff {
    class BinaryWriter {
    public:
        BinaryWriter();
        virtual ~BinaryWriter();

        virtual int write(uint8_t* const& src, const int& count, const int& offset = 0) = 0;
    };
}

#endif
