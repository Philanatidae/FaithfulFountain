/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_RENDER_GROUP_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_RENDER_GROUP_HPP

#include <stdint.h>

namespace ff {
    typedef uint16_t RenderGroup;

    constexpr RenderGroup RENDER_GROUP_1 = 0x1;
    constexpr RenderGroup RENDER_GROUP_2 = 0x2;
    constexpr RenderGroup RENDER_GROUP_3 = 0x4;
    constexpr RenderGroup RENDER_GROUP_4 = 0x8;
    constexpr RenderGroup RENDER_GROUP_5 = 0x10;
    constexpr RenderGroup RENDER_GROUP_6 = 0x20;
    constexpr RenderGroup RENDER_GROUP_7 = 0x40;
    constexpr RenderGroup RENDER_GROUP_8 = 0x80;
    constexpr RenderGroup RENDER_GROUP_9 = 0x100;
    constexpr RenderGroup RENDER_GROUP_10 = 0x200;
    constexpr RenderGroup RENDER_GROUP_11 = 0x400;
    constexpr RenderGroup RENDER_GROUP_12 = 0x800;
    constexpr RenderGroup RENDER_GROUP_13 = 0x1000;
    constexpr RenderGroup RENDER_GROUP_14 = 0x2000;
    constexpr RenderGroup RENDER_GROUP_15 = 0x4000;
    constexpr RenderGroup RENDER_GROUP_16 = 0x8000;

    constexpr RenderGroup RENDER_GROUP_MASK_ALL = 0xFFFF;
    constexpr RenderGroup RENDER_GROUP_MASK_NONE = 0;
}

#endif
