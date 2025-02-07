/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ACTORS_ACTOR_TYPES_HPP
#define _FAITHFUL_FOUNTAIN_ACTORS_ACTOR_TYPES_HPP

#include <stdint.h>
#include <limits>

namespace ff {
    using ActorID = uint32_t;
    using ActorVersion = uint32_t;

    constexpr ActorID FF_ACTOR_INVALID = std::numeric_limits<ActorID>::max();
    constexpr ActorVersion FF_ACTOR_MAX_VERSION = std::numeric_limits<ActorVersion>::max();

    using Actor_t = uint64_t;

    inline ActorID convertActorToID(Actor_t actor) {
        // 32-bit actor ID is stored in the most significant 32 bits
        return static_cast<ActorID>(actor >> 32);
    }
    inline ActorVersion convertActorToVersion(Actor_t actor) {
        // 32-bit actor version is stored in the least significant 32 bits
        return static_cast<ActorVersion>(actor & 0xFFFFFFFF);
    }

    inline Actor_t convertActorIDAndVersionToActor(ActorID id, ActorVersion version) {
        return static_cast<Actor_t>(id) << 32 | static_cast<Actor_t>(version);
    }

    inline bool isActorNull(Actor_t actor) {
        return convertActorToID(actor) == FF_ACTOR_INVALID;
    }

    constexpr Actor_t NullActor = (((Actor_t)FF_ACTOR_INVALID) << 32) | (Actor_t)(FF_ACTOR_MAX_VERSION);
}

#endif
