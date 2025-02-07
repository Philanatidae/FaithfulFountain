/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_INDEX_BUFFER_TYPE_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_INDEX_BUFFER_TYPE_HPP

#include <stdint.h>

namespace ff {
    enum class IndexType {
        UINT32,
        UINT16
    };

    template<IndexType> struct IndexTypeUnderlyingType;

    template<> struct IndexTypeUnderlyingType<IndexType::UINT32> {
        using type = uint32_t;
    };
    template<> struct IndexTypeUnderlyingType<IndexType::UINT16> {
        using type = uint16_t;
    };
}

#endif
