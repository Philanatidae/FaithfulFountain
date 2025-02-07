/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_COMMANDS_RENDER_RENDER_CMD_HPP
#define _FF_COMMANDS_RENDER_RENDER_CMD_HPP

#include <ff/messages/CmdHelpers.hpp>

namespace ff {
FF_CMD_DEFINE_2_R0(RenderCmd,
    "cmd_render",
    "Instruct the render core to render the game.",
    float, betweenFrameAlpha,
    float, dt,
    "Alpha: %s, dt: %s",
    (betweenFrameAlpha, dt))
}

#endif

