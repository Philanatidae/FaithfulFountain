/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_COMMANDS_RENDER_RENDER_IMGUI_CMD_HPP
#define _FF_COMMANDS_RENDER_RENDER_IMGUI_CMD_HPP

#include <ff/messages/CmdHelpers.hpp>

namespace ff {
FF_CMD_DEFINE_3_R0(RenderImGuiCmd,
    "cmd_render_imgui",
    "Render ImGui layer to the screen",
    float, betweenFrameAlpha,
    float, dt,
    std::string, debugCamera,
    "Alpha: %s, dt: %s, Camera: %s",
    (betweenFrameAlpha, dt, debugCamera))
}

#endif


