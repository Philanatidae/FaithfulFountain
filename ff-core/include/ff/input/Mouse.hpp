/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_INPUT_MOUSE_HPP
#define _FAITHFUL_FOUNTAIN_INPUT_MOUSE_HPP

namespace ff {
    enum class MouseButton: int {
        UNKNOWN = -1,

        PRIMARY = 0,
        SECONDARY = 1,
        MIDDLE = 2,

        COUNT
    };

    enum class MouseCursor: int {
        UNKNOWN = -1,

        DEFAULT = 0,
        IBEAM,
        HAND,
        RESIZE_EW,
        RESIZE_NS,
        RESIZE_ALL,
        RESIZE_NESW,
        RESIZE_NWSE,
        NOT_ALLOWED,

        COUNT
    };

    enum class MouseMoveType: int {
        ABSOLUTE = 0,
        RELATIVE
    };
}

#endif
