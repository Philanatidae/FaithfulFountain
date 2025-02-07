/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/actors/IterableActorSet.hpp>

#include <ff/actors/ActorManager.hpp>

namespace ff {
    IterableActorSet::IterableActorSet(ActorManager* const& actorManagerPtr,
        std::vector<Actor_t>* const& actorListPtr,
        uint32_t* const& deletedActorCount)
        :_actorManagerPtr(actorManagerPtr),
        _actors(actorListPtr),
        _deletedActorCount(deletedActorCount) {
    }
    IterableActorSet::~IterableActorSet() {
    }

    void IterableActorSet::each(const std::function<void(Actor_t)>& iterFn) const {
        // Cases we need to consider which may invalidate iterators or component addresses:
        // 1) Actor created
        // - ActorManager _actors will could be reallocated, but since we are looping with indices we are okay,
        //      we don't actually care where the new actor is in memory.
        // - Similar thing happens in components. Actual packed component data doesn't get reallocated yet.
        // 2) Actor destroyed
        // - ActorManager _actors will never be reallocated.
        // 3) Component added
        // - If adding to this entity, we're okay, since we couldn't have had the component in the first place by definition.
        // - If adding to another entity, we can potentially re-allocate the packed component data. The only alternative to this
        //      would be to add support for paging instead of a std::vector.
        // 4) Component removed
        // - If we swap last-with-the-removed component, we're okay, since the memory position of the current entity is valid.

        for(int i = _actors->size() - 1; i >= 0; --i) {
            // Detect if the actor is invalid
            // A valid actor has their id equal to their place in the main set.
            if(!_actorManagerPtr->isActorValid((*_actors)[i])) {
                continue;
            }
            iterFn((*_actors)[i]);
        }
    }
    bool IterableActorSet::contains(const Actor_t& actor) const {
        return std::find(_actors->begin(), _actors->end(), actor) != _actors->end();
    }
    int IterableActorSet::count() const {
        return (int)(_actors->size() - (_deletedActorCount == nullptr ? 0 : *_deletedActorCount));
    }
}
