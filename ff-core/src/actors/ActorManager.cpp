/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/actors/ActorManager.hpp>

#include <ff/Console.hpp>

namespace ff {
    ActorManager::ActorManager()
        :_nextFreeActor(FF_ACTOR_INVALID),
        _freeActorCount(0),
        _actorsSet(this, &_actors, &_freeActorCount) {
    }
    ActorManager::~ActorManager() {
    }

    Actor_t ActorManager::createActor() {
        Actor_t actor;
        if(_freeActorCount == 0) {
            FF_ASSERT(_actors.size() < FF_ACTOR_INVALID, "Maximum number of simultaneous actors reached (%s).", FF_ACTOR_INVALID);
            for(auto& pair : _componentMaps) {
                pair.second->addEmptyActor();
            }
            _componentMaskSet.addEmptyActor();
            _actors.push_back(convertActorIDAndVersionToActor(_actors.size(), 0));
            actor = _actors.back();
        } else {
            ActorID actorID = _nextFreeActor;
            _nextFreeActor = convertActorToID(_actors[actorID]);
            --_freeActorCount;

            ActorVersion version = convertActorToVersion(_actors[actorID]);
            FF_ASSERT(version < FF_ACTOR_MAX_VERSION, "Maximum number of actor versions reached (%s).", FF_ACTOR_MAX_VERSION);
            ++version;
            actor = _actors[actorID] = convertActorIDAndVersionToActor(actorID, version);
        }

        for(auto& pair : _familyActorSets) {
            if(pair.first.matches(FF_ACTOR_COMPONENT_MASK_EMPTY)) {
                pair.second->addActor(actor);
            }
        }
        return actor;
    }
    bool ActorManager::isActorAlive(const Actor_t& actor) const {
        return convertActorToID(actor) < _actors.size() && _actors[convertActorToID(actor)] == actor;
    }

    void ActorManager::destroyActor(const Actor_t& actor) {
        if(!isActorAlive(actor)) {
            return;
        }

        ComponentType prevMask = getComponentMask(actor);

        for(auto& pair : _componentMaps) {
            pair.second->removeComponent(actor);
        }
        _componentMaskSet.clearMask(actor);

        updateFamilyActorMapsForActor(actor, prevMask);

        if(_freeActorCount == 0) {
            _nextFreeActor = convertActorToID(actor);
            _actors[convertActorToID(actor)] = convertActorIDAndVersionToActor(FF_ACTOR_INVALID, convertActorToVersion(actor));
        } else {
            ActorID lastDeletedActor = _nextFreeActor;
            while(convertActorToID(_actors[lastDeletedActor]) != FF_ACTOR_INVALID) {
                lastDeletedActor = convertActorToID(_actors[lastDeletedActor]);
            }
            _actors[lastDeletedActor] = convertActorIDAndVersionToActor(convertActorToID(actor),
                convertActorToVersion(_actors[lastDeletedActor]));
            _actors[convertActorToID(actor)] = convertActorIDAndVersionToActor(FF_ACTOR_INVALID, convertActorToVersion(actor));
        }

        _freeActorCount++;
    }
    void ActorManager::destroyAllActors() {
        if(_freeActorCount == _actors.size()) {
            return;
        }

        for(auto& pair : _componentMaps) {
            pair.second->removeAllActors();
        }
        for(auto& pair : _familyActorSets) {
            pair.second->removeAllActors();
        }
        _freeActorCount = _actors.size();
        _nextFreeActor = 0;

        _actors[_actors.size() - 1] = convertActorIDAndVersionToActor(FF_ACTOR_INVALID,
            convertActorToVersion(_actors[_actors.size() - 1]));
        for(int i = _actors.size() - 2; i >= 0; --i) {
            _actors[i] = convertActorIDAndVersionToActor(i + 1,
                convertActorToVersion(_actors[i]));
        }
    }
    void ActorManager::destroyAllActorsFor(Family const& family) {
        getActorsFor(family).each([this](Actor_t actor) -> void {
            destroyActor(actor);
        });
    }

    const IterableActorSet& ActorManager::getActors() {
        return _actorsSet;
    }
    const IterableActorSet& ActorManager::getActorsFor(const Family& family) {
        if(_familyActorSets.find(family) == _familyActorSets.end()) {
            _familyActorSets.emplace(family, std::make_unique<FamilyActorSet>(this, family));
            FamilyActorSet& familyActorSet = *_familyActorSets.at(family);
            getActors().each([this, family, &familyActorSet](Actor_t actor) {
                if(family.matches(this, actor)) {
                    familyActorSet.addActor(actor);
                }
            });
        }
        return _familyActorSets.at(family)->getActors();
    }

    ComponentType ActorManager::getComponentMask(const Actor_t& actor) const {
        return _componentMaskSet.getComponentMask(actor);
    }

    void ActorManager::updateFamilyActorMapsForActor(const Actor_t& actor, const ComponentType& prevMask) {
        for(auto& pair : _familyActorSets) {
            if(!pair.first.matches(prevMask)
                && pair.first.matches(this, actor)) {
                pair.second->addActor(actor);
                continue;
            }
            if(pair.first.matches(prevMask)
                && !pair.first.matches(this, actor)) {
                pair.second->removeActor(actor);
                continue;
            }
            if(!isActorAlive(actor)
                && pair.first.matches(prevMask)
                && pair.second->hasActor(actor)) {
                pair.second->removeActor(actor);
                continue;
            }
        }
    }

    bool ActorManager::isActorValid(Actor_t const& actor) {
        ff::ActorID id = convertActorToID(actor);
        if(id > _actors.size() - 1) {
            return false;
        }
        return id == convertActorToID(_actors[id]);
    }
}
