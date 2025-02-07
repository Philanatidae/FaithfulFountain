/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_EVENTS_INPUT_GAMEPAD_DISCONNECTED_EVENT_HPP
#define _FAITHFUL_FOUNTAIN_EVENTS_INPUT_GAMEPAD_DISCONNECTED_EVENT_HPP

#include <ff/messages/Event.hpp>

#include <ff/input/Gamepad.hpp>
#include <tinyformat/tinyformat.h>

namespace ff {

    FF_EVENT_DEFINE_2(GamepadDisconnectedEvent,
        "evt_gamepad_disconnected",
        GamepadID_t, id,
        uint64_t, systemId,
        "%i (System ID %i)",
        id, systemId);

}

#endif
