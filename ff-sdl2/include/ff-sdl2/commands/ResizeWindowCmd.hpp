/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_SDL2_RESIZE_WINDOW_CMD_HPP
#define _FAITHFUL_FOUNTAIN_SDL2_RESIZE_WINDOW_CMD_HPP

#include <ff/messages/CmdHelpers.hpp>

namespace ff {

    FF_CMD_DEFINE_2_R0(ResizeWindowCmd,
        "cmd_sdl2_resize_window",
        "Resize the window",
        int, width,
        int, height,
        "<%s, %s>",
        (width, height)
    )
}

#endif
