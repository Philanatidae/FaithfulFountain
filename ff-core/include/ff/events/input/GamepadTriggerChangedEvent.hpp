/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_EVENTS_INPUT_GAMEPAD_TRIGGER_CHANGED_EVENT_HPP
#define _FAITHFUL_FOUNTAIN_EVENTS_INPUT_GAMEPAD_TRIGGER_CHANGED_EVENT_HPP

namespace ff {

#include <ff/messages/Event.hpp>

#include <ff/input/Gamepad.hpp>
#include <tinyformat/tinyformat.h>

#include <glm/glm.hpp>

namespace ff {

    FF_EVENT_DEFINE_4(GamepadTriggerChangedEvent,
        "evt_gamepad_trigger_changed",
        GamepadID_t, id,
        uint64_t, systemId,
        GamepadTrigger, trigger,
        glm::vec2, value,
        "%s: %s (Gamepad %i, System ID %i)",
        (int)trigger, value, id,  systemId);

}

#endif
