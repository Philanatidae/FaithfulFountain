/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ACTORS_FAMILYACTORSET_HPP
#define _FAITHFUL_FOUNTAIN_ACTORS_FAMILYACTORSET_HPP

#include <ff/actors/Actor.hpp>
#include <ff/actors/Family.hpp>
#include <ff/actors/IterableActorSet.hpp>
#include <vector>
#include <stdint.h>

namespace ff {
    class FamilyActorSet final {
    public:
        FamilyActorSet(ActorManager* const& actorManagerPtr,
            const Family& family);
        ~FamilyActorSet() = default;

        const Family& getFamily() const;

        void addActor(const Actor_t& actor);
        void removeActor(const Actor_t& actor);
        void removeAllActors();
        bool hasActor(const Actor_t& actor) const;

        const IterableActorSet& getActors() const;

    private:
        ActorManager* const _actorManagerPtr;
        Family _family;
        std::vector<Actor_t> _actors;
        IterableActorSet _iterableActors;
        uint32_t _nextOpenIndex;
        uint32_t _emptySpots;
    };
}

#endif
