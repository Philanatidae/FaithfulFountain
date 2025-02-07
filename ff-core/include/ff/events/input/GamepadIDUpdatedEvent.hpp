/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_EVENTS_INPUT_GAMEPAD_ID_UPDATED_EVENT_HPP
#define _FAITHFUL_FOUNTAIN_EVENTS_INPUT_GAMEPAD_ID_UPDATED_EVENT_HPP

#include <ff/messages/Event.hpp>

#include <ff/input/Gamepad.hpp>
#include <tinyformat/tinyformat.h>
#include <bitset>

namespace ff {

    FF_EVENT_DEFINE_3(GamepadIDUpdatedEvent,
        "evt_gamepad_id_updated",
        GamepadID_t, prevId,
        uint64_t, systemId,
        GamepadID_t, id,
        "Previous ID: %i (System ID: %s) -> %i",
        prevId,
        systemId,
        id);

}

#endif
