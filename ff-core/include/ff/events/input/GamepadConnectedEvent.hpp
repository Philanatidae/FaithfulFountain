/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_EVENTS_INPUT_GAMEPAD_CONNECTED_EVENT_HPP
#define _FAITHFUL_FOUNTAIN_EVENTS_INPUT_GAMEPAD_CONNECTED_EVENT_HPP

#include <ff/messages/Event.hpp>

#include <ff/input/Gamepad.hpp>
#include <tinyformat/tinyformat.h>
#include <bitset>

namespace ff {

FF_EVENT_DEFINE_5(GamepadConnectedEvent,
    "evt_gamepad_conected",
    GamepadID_t, id,
    uint64_t, systemID,
    std::string, name,
    GamepadType, type,
    GamepadCapabilities_t, capabilities,
    "%i (%s), Type: %s, Capabilities: %s, System ID: %s",
    id, name, (int)type,
    std::bitset<8*sizeof(GamepadCapabilities_t)>(capabilities),
    systemID);

}

#endif
