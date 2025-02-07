/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_EVENTS_INPUT_GAMEPAD_STICK_CHANGED_EVENT_HPP
#define _FAITHFUL_FOUNTAIN_EVENTS_INPUT_GAMEPAD_STICK_CHANGED_EVENT_HPP

#include <ff/messages/Event.hpp>

#include <ff/input/Gamepad.hpp>
#include <tinyformat/tinyformat.h>

#include <glm/glm.hpp>

namespace ff {

    FF_EVENT_DEFINE_4(GamepadStickChangedEvent,
        "evt_gamepad_stick_changed",
        GamepadID_t, id,
        uint64_t, systemId,
        GamepadStick, stick,
        glm::vec2, value,
        "%s: <%s, %s> (Gamepad %i, System ID %i)",
        (int)stick, value.x, value.y, id, systemId);

}

#endif
