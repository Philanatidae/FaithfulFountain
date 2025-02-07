/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ACTORS_ACTORMANAGER_HPP
#define _FAITHFUL_FOUNTAIN_ACTORS_ACTORMANAGER_HPP

#include <vector>
#include <unordered_map>
#include <memory>

#include <ff/actors/Actor.hpp>

#include <ff/actors/Component.hpp>
#include <ff/actors/ComponentMap.hpp>
#include <ff/actors/ComponentMaskSet.hpp>

#include <ff/actors/IterableActorSet.hpp>

#include <ff/actors/Family.hpp>
#include <ff/actors/FamilyActorSet.hpp>

namespace ff {
    class FamilyActorSet;

    class ActorManager final {
    friend class Actor;
    friend class IterableActorSet;

    public:
        ActorManager();
        ~ActorManager();

        Actor_t createActor();
        bool isActorAlive(const Actor_t& actor) const;

        void destroyActor(const Actor_t& actor);
        void destroyAllActors();
        void destroyAllActorsFor(Family const& family);

        const IterableActorSet& getActors();
        const IterableActorSet& getActorsFor(const Family& family);

        template<typename T, typename... Args>
        T& addComponent(const Actor_t& actor, Args&&... args);
        template<typename T>
        bool hasComponent(const Actor_t& actor) const;
        template<typename T>
        T& getComponent(const Actor_t& actor);
        template<typename T>
        const T& getComponent(const Actor_t& actor) const;
        template<typename T>
        void removeComponent(const Actor_t& actor);
        ComponentType getComponentMask(const Actor_t& actor) const;

    private:
        std::vector<Actor_t> _actors;
        ActorID _nextFreeActor;
        uint32_t _freeActorCount;
        IterableActorSet _actorsSet;

        std::unordered_map<ComponentType, std::unique_ptr<IComponentMap>> _componentMaps;
        ComponentMaskSet _componentMaskSet;

        int _deferredManagerModificationStack;
        void beginDeferringManagerModifications();
        void endDeferringManagerModifications();

        template<typename T>
        void initComponentMapForComponent();

        std::unordered_map<Family, std::unique_ptr<FamilyActorSet>> _familyActorSets;

        void updateFamilyActorMapsForActor(const Actor_t& actor, const ComponentType& prevMask);

        bool isActorValid(Actor_t const& actor);
    };
}

#include <type_traits>

namespace ff {
    template<typename T, typename... Args>
    T& ActorManager::addComponent(const Actor_t& actor, Args&&... args) {
        ComponentType type = Component<T>::getType();
        if(_componentMaps.find(type) == _componentMaps.end()) {
            initComponentMapForComponent<T>();
        }
        ComponentMap<T>& map = static_cast<ComponentMap<T>&>(*_componentMaps.at(type));
        ComponentType mask = getComponentMask(actor);
        map.addComponent(actor, std::forward<Args>(args)...);
        _componentMaskSet.addComponent(actor, type);
        updateFamilyActorMapsForActor(actor, mask);
        return getComponent<T>(actor);
    }
    template<typename T>
    bool ActorManager::hasComponent(const Actor_t& actor) const {
        return _componentMaskSet.hasComponent(actor, Component<T>::getType());
    }
    template<typename T>
    T& ActorManager::getComponent(const Actor_t& actor) {
        ComponentType type = Component<T>::getType();
        if(_componentMaps.find(type) == _componentMaps.end()) {
            initComponentMapForComponent<T>();
        }
        ComponentMap<T>& map = static_cast<ComponentMap<T>&>(*_componentMaps.at(type));
        return map.getComponent(actor);
    }
    template<typename T>
    const T& ActorManager::getComponent(const Actor_t& actor) const {
        ComponentType type = Component<T>::getType();
        FF_ASSERT(_componentMaps.find(type) != _componentMaps.end(), "Component type not registered with ActorManager.");
        const ComponentMap<T>& map = static_cast<const ComponentMap<T>&>(*_componentMaps.at(type));
        return map.getComponent(actor);
    }
    template<typename T>
    void ActorManager::removeComponent(const Actor_t& actor) {
        ComponentType type = Component<T>::getType();
        if(_componentMaps.find(type) == _componentMaps.end()) {
            initComponentMapForComponent<T>();
        }
        ComponentType mask = getComponentMask(actor);
        ComponentMap<T>& map = static_cast<ComponentMap<T>&>(*_componentMaps.at(type));
        map.removeComponent(actor);
        _componentMaskSet.removeComponent(actor, type);
        updateFamilyActorMapsForActor(actor, mask);
    }

    template<typename T>
    void ActorManager::initComponentMapForComponent() {
        static_assert(std::is_base_of<Component<T>, T>::value, "T must extend off of Component<T>.");

        ComponentType type = Component<T>::getType();
        _componentMaps.emplace(type, std::make_unique<ComponentMap<T>>());
        _componentMaps[type]->addEmptyActors((int)_actors.size());
    }
}

#endif
