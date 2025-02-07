/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_COMMANDS_SHUTDOWN_HPP
#define _FAITHFUL_FOUNTAIN_COMMANDS_SHUTDOWN_HPP

#include <ff/messages/CmdHelpers.hpp>

namespace ff {
FF_CMD_DEFINE_0_R0(ShutdownCmd,
    "cmd_shutdown",
    "Shut down the game");
}

#endif
