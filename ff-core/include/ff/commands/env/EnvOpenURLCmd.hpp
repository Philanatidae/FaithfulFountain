/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_COMMANDS_ENV_ENV_OPEN_URL_HPP
#define _FAITHFUL_FOUNTAIN_COMMANDS_ENV_ENV_OPEN_URL_HPP

#include <ff/messages/CmdHelpers.hpp>

namespace ff {
    FF_CMD_DEFINE_1_R0(EnvOpenURLCmd,
        "cmd_env_open_url",
        "Open a URL in the Environment",
        std::string, address,
        "%s",
        (address));
}

#endif
