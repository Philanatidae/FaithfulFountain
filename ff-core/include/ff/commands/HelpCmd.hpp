/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_COMMANDS_HELP_CMD_HPP
#define _FF_COMMANDS_HELP_CMD_HPP

#include <ff/messages/CmdHelpers.hpp>
#include <optional>

namespace ff {
    FF_CMD_DEFINE_1_R0(HelpCmd,
        "cmd_help",
        "Print help text to the Console",
        std::optional<std::string>, cmd,
        "%s",
        (cmd == std::nullopt ? "?" : cmd.value()));
}

#endif

