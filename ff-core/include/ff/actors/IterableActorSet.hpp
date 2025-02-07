/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ACTORS_ITERABLEACTORSET_HPP
#define _FAITHFUL_FOUNTAIN_ACTORS_ITERABLEACTORSET_HPP

#include <functional>

#include <ff/actors/Actor.hpp>
#include <vector>

namespace ff {
    class ActorManager;

    class IterableActorSet {
    public:
        IterableActorSet(ActorManager* const& actorManagerPtr,
            std::vector<Actor_t>* const& actorListPtr,
            uint32_t* const& deletedActorCount = nullptr);
        virtual ~IterableActorSet();

        void each(const std::function<void(Actor_t)>& iterFn) const;
        bool contains(const Actor_t& actor) const;
        int count() const;

    private:
        ActorManager* const _actorManagerPtr;
        std::vector<Actor_t>* _actors;
        uint32_t* _deletedActorCount;
    };
}

#include <ff/actors/ActorManager.hpp>

#endif
