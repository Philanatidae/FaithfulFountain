/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_COMMANDS_DEV_POPULATE_IM_GUI_FRAME_EVENT_HPP
#define _FAITHFUL_FOUNTAIN_COMMANDS_DEV_POPULATE_IM_GUI_FRAME_EVENT_HPP

#include <ff/messages/Event.hpp>

namespace ff {
    FF_EVENT_DEFINE_1(PopulateImGuiFrameEvent,
        "evt_imgui_populate_frame",
        std::string, debugCamera,
        "%s",
        (debugCamera));
}

#endif
