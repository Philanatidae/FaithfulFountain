/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_COMMANDS_ENV_ENV_END_TEXT_INPUT_COMMAND_HPP
#define _FAITHFUL_FOUNTAIN_COMMANDS_ENV_ENV_END_TEXT_INPUT_COMMAND_HPP

#include <ff/messages/CmdHelpers.hpp>

namespace ff {
    FF_CMD_DEFINE_0_R0(EnvEndTextInputCmd,
        "cmd_env_end_text_input",
        "End Environment text input");
}

#endif
