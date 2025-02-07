/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_EVENTS_GAME_SHUTDOWN_EVENT_HPP
#define _FAITHFUL_FOUNTAIN_EVENTS_GAME_SHUTDOWN_EVENT_HPP

#include <ff/messages/Event.hpp>
#include <tinyformat/tinyformat.h>
#include <glm/glm.hpp>

namespace ff {
    FF_EVENT_DEFINE_0(GameShutdownEvent,
        "evt_game_shutdown");
}

#endif

