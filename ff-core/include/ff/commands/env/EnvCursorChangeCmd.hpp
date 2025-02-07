/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_COMMANDS_ENV_ENV_CURSOR_CHANGE_HPP
#define _FAITHFUL_FOUNTAIN_COMMANDS_ENV_ENV_CURSOR_CHANGE_HPP

#include <ff/messages/CmdHelpers.hpp>
#include <ff/input/Mouse.hpp>

#include <tinyformat/tinyformat.h>

namespace ff {
    FF_CMD_DEFINE_1_R0(EnvCursorChangeCmd,
        "cmd_env_cursor_change",
        "Change cursor",
        MouseCursor, cursor,
        "%s",
        ((int)cursor));
}
 
#endif
