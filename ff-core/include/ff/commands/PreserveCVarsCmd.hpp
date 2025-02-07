/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_COMMANDS_PRESERVE_CVARS_COMMAND_HPP
#define _FAITHFUL_FOUNTAIN_COMMANDS_PRESERVE_CVARS_COMMAND_HPP

#include <ff/messages/CmdHelpers.hpp>

namespace ff {
    FF_CMD_DEFINE_0_R0(PreserveCVarsCmd,
        "cmd_preserve_cvars",
        "Preserve CVars to `cvars.ini`");
}

#endif
