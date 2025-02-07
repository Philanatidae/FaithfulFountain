/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_COMMANDS_INPUT_GAMEPAD_UPDATE_ID_COMMAND_HPP
#define _FAITHFUL_FOUNTAIN_COMMANDS_INPUT_GAMEPAD_UPDATE_ID_COMMAND_HPP

#include <ff/messages/CmdHelpers.hpp>

#include <ff/input/Gamepad.hpp>
#include <tinyformat/tinyformat.h>

namespace ff {
    FF_CMD_DEFINE_2_R0(GamepadUpdateIDCmd,
        "cmd_gamepad_update_id",
        "Update a gamepad ID",
        uint64_t, systemId,
        GamepadID_t, id,
        "System %i, ID: %i",
        (systemId, id));
}

#endif
