/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_EVENTS_COLLISION_COLLISION_START_EVENT_HPP
#define _FAITHFUL_FOUNTAIN_EVENTS_COLLISION_COLLISION_START_EVENT_HPP

#include <ff/messages/Event.hpp>
#include <tinyformat/tinyformat.h>
#include <ff/actors/Actor.hpp>
#include <glm/glm.hpp>

namespace ff {
    FF_EVENT_DEFINE_5(CollisionStartEvent,
        "evt_collision_start",
        Actor_t, actorA,
        glm::vec2, normA,
        Actor_t, actorB,
        glm::vec2, normB,
        float, penetration,
        "%s<-->%s (penetration: %s)",
        actorA, actorB, penetration);
}

#endif
