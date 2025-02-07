/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_EVENTS_MOUSE_BUTTON_UP_EVENT_HPP
#define _FAITHFUL_FOUNTAIN_EVENTS_MOUSE_BUTTON_UP_EVENT_HPP

#include <ff/input/Mouse.hpp>
#include <ff/messages/Event.hpp>
#include <tinyformat/tinyformat.h>
#include <glm/glm.hpp>

namespace ff {
    
    FF_EVENT_DEFINE_2(MouseButtonUpEvent,
        "evt_mouse_button_up",
        MouseButton, button,
        glm::vec2, position,
        "%s @ <%s, %s>",
        (int)button,
        position.x, position.y);
}

#endif
