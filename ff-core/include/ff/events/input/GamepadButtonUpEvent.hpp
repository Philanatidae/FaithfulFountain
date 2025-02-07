/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_EVENTS_INPUT_GAMEPAD_BUTTON_UP_EVENT_HPP
#define _FAITHFUL_FOUNTAIN_EVENTS_INPUT_GAMEPAD_BUTTON_UP_EVENT_HPP

#include <ff/messages/Event.hpp>

#include <ff/input/Gamepad.hpp>
#include <tinyformat/tinyformat.h>

namespace ff {

    FF_EVENT_DEFINE_3(GamepadButtonUpEvent,
        "evt_gamepad_button_up",
        GamepadID_t, id,
        uint64_t, systemId,
        GamepadButton, button,
        "%s (Gamepad %i, System ID %i)",
        (int)button, id, systemId);

}

#endif
