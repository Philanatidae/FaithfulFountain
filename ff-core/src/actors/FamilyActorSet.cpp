/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/actors/FamilyActorSet.hpp>

#include <ff/Console.hpp>

namespace ff {
    FamilyActorSet::FamilyActorSet(ActorManager* const& actorManagerPtr,
        const Family& family)
        :_actorManagerPtr(actorManagerPtr),
        _family(family),
        _iterableActors(_actorManagerPtr, &_actors, &_emptySpots),
        _nextOpenIndex(0),
        _emptySpots(0) {
    }

    const Family& FamilyActorSet::getFamily() const {
        return _family;
    }

    void FamilyActorSet::addActor(const Actor_t& actor) {
        if (_emptySpots > 0) {
            Actor_t index = _nextOpenIndex;
            _nextOpenIndex = convertActorToID(_actors[_nextOpenIndex]);
            _actors[index] = actor;
            _emptySpots--;
        } else {
            _actors.push_back(actor);
        }
    }
    void FamilyActorSet::removeActor(const Actor_t& actor) {
        auto it = std::find(_actors.begin(), _actors.end(), actor);
        FF_ASSERT(it != _actors.end(), "Actor not found in family.");
        int indexOfActor = it - _actors.begin();

        if(_emptySpots > 0) {
            Actor_t lastOpenIndex = _nextOpenIndex;
            while(convertActorToID(_actors[lastOpenIndex]) != FF_ACTOR_INVALID) {
                lastOpenIndex = convertActorToID(_actors[lastOpenIndex]);
            }

            _actors[lastOpenIndex] = convertActorIDAndVersionToActor(indexOfActor, FF_ACTOR_MAX_VERSION);
            _actors[indexOfActor] = convertActorIDAndVersionToActor(FF_ACTOR_INVALID, FF_ACTOR_MAX_VERSION);
        } else {
            _nextOpenIndex = indexOfActor;
            _actors[indexOfActor] = convertActorIDAndVersionToActor(FF_ACTOR_INVALID, FF_ACTOR_MAX_VERSION);
        }
        _emptySpots++;
    }
    void FamilyActorSet::removeAllActors() {
        _actors.back() = convertActorIDAndVersionToActor(FF_ACTOR_INVALID, FF_ACTOR_MAX_VERSION);
        for(int i = 0; i < _actors.size() - 1; i++) {
            _actors[i] = convertActorIDAndVersionToActor(i + 1, FF_ACTOR_MAX_VERSION);
        }
    }
    bool FamilyActorSet::hasActor(const Actor_t& actor) const {
        return std::find(_actors.begin(), _actors.end(), actor) != _actors.end();
    }

    const IterableActorSet& FamilyActorSet::getActors() const {
        return _iterableActors;
    }
}
