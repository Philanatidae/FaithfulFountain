/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_COMMANDS_INPUT_RELATIVE_MOUSE_MOVEMENT_CMD_HPP
#define _FF_COMMANDS_INPUT_RELATIVE_MOUSE_MOVEMENT_CMD_HPP

#include <ff/messages/CmdHelpers.hpp>

namespace ff {

FF_CMD_DEFINE_0_R0(RelativeMouseMovementCmd,
    "cmd_relative_mouse_movement",
    "Report mouse movement relative to the current position of the mouse cursor");

}

#endif

