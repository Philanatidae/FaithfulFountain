/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_COMMANDS_SET_CMD_HPP
#define _FF_COMMANDS_SET_CMD_HPP

#include <ff/messages/CmdHelpers.hpp>
#include <optional>

namespace ff {
    FF_CMD_DEFINE_2_R0(SetCmd,
        "cmd_set",
        "Set the value of a CVar to the Console",
        std::string, cvar,
        std::string, val,
        "%s := %s",
        (cvar, val));
}

#endif

