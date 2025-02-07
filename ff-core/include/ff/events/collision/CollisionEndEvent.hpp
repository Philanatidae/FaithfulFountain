/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_EVENTS_COLLISION_COLLISION_END_EVENT_HPP
#define _FAITHFUL_FOUNTAIN_EVENTS_COLLISION_COLLISION_END_EVENT_HPP

#include <ff/messages/Event.hpp>
#include <tinyformat/tinyformat.h>
#include <ff/actors/Actor.hpp>

namespace ff {
    FF_EVENT_DEFINE_2(CollisionEndEvent,
        "evt_collision_end",
        Actor_t, actorA,
        Actor_t, actorB,
        "%s<-->%s",
        actorA, actorB);
}

#endif
