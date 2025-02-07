/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_COMMANDS_ENV_ENV_BEGIN_TEXT_INPUT_COMMAND_HPP
#define _FAITHFUL_FOUNTAIN_COMMANDS_ENV_ENV_BEGIN_TEXT_INPUT_COMMAND_HPP

#include <ff/messages/CmdHelpers.hpp>
#include <tinyformat/tinyformat.h>

namespace ff {
    FF_CMD_DEFINE_1_R0(EnvBeginTextInputCmd,
        "cmd_env_begin_text_input",
        "Begin Environment text input",
        std::string, content,
        "Content: %s",
        (content));
}

#endif
