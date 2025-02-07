/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_EVENTS_MOUSE_MOVE_EVENT_HPP
#define _FAITHFUL_FOUNTAIN_EVENTS_MOUSE_MOVE_EVENT_HPP

#include <ff/messages/Event.hpp>
#include <tinyformat/tinyformat.h>
#include <glm/glm.hpp>
#include <ff/input/Mouse.hpp>
#include <magic_enum.hpp>

namespace ff {
    FF_EVENT_DEFINE_2(MouseMoveEvent,
        "evt_mouse_move",
        glm::vec2, position,
        MouseMoveType, type,
        "<%s, %s> (%s)",
        position.x, position.y, magic_enum::enum_name(type).data());
}

#endif
