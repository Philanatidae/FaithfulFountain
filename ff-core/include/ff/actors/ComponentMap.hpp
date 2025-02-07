/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ACTORS_COMPONENTMAP_HPP
#define _FAITHFUL_FOUNTAIN_ACTORS_COMPONENTMAP_HPP

#include <ff/actors/Actor.hpp>

#include <vector>
#include <memory>
#include <array>

#include <ff/events/actors/ComponentRemovedEvent.hpp>

namespace ff {
    struct IComponentMap {
        virtual ~IComponentMap() = default;

        virtual void addEmptyActor() = 0;
        virtual void addEmptyActors(const int& count) = 0;
        virtual void removeAllActors() = 0;

        virtual void removeComponent(const Actor_t& actor) = 0;
        virtual bool hasComponent(const Actor_t& actor) const = 0;
    };

    constexpr size_t FF_COMPONENT_MAP_PAGE_SIZE = 256;

    template<typename T>
    class ComponentMap : public IComponentMap {
    public:
        ComponentMap();
        virtual ~ComponentMap() = default;
        
        void addEmptyActor() override;
        void addEmptyActors(const int& count) override;
        virtual void removeAllActors() override;

        template<typename... Args>
        void addComponent(const Actor_t& actor, Args&&... args);
        void removeComponent(const Actor_t& actor) override;
        bool hasComponent(const Actor_t& actor) const override;
        T& getComponent(const Actor_t& actor);
        const T& getComponent(const Actor_t& actor) const;

    private:
        std::vector<ActorID> _actorIndices;
        
        std::vector<ActorID> _actorList;
        std::vector<std::unique_ptr<std::array<T, FF_COMPONENT_MAP_PAGE_SIZE>>> _componentListPages;
        int _abandondedComponentCount;
        int _nextAbandonedComponent;

        void addNewPage();
    };
}

#include <ff/Console.hpp>
#include <ff/Locator.hpp>

namespace ff {
    template<typename T>
    ComponentMap<T>::ComponentMap()
        :_abandondedComponentCount(0),
        _nextAbandonedComponent(0) {
    }

    template<typename T>
    void ComponentMap<T>::addEmptyActor() {
        _actorIndices.push_back(FF_ACTOR_INVALID);
    }
    template<typename T>
    void ComponentMap<T>::addEmptyActors(const int& count) {
        _actorIndices.resize(_actorIndices.size() + count, FF_ACTOR_INVALID);
    }
    template<typename T>
    void ComponentMap<T>::removeAllActors() {
        for(int i = 0; i < _actorIndices.size(); i++) {
            removeComponent(_actorIndices[i]);
        }
    }

    template<typename T>
    template<typename... Args>
    void ComponentMap<T>::addComponent(const Actor_t& actor, Args&&... args) {
        FF_ASSERT(!hasComponent(actor), "Actor %s already has component.", actor);

        ActorID actorID = convertActorToID(actor);
        if (_actorIndices[actorID] == FF_ACTOR_INVALID) {
            if(_abandondedComponentCount == 0) {
                addNewPage();
            }
            FF_ASSERT(_abandondedComponentCount > 0, "Error in adding new component list page.");
            
            int index = _nextAbandonedComponent;
            _actorIndices[actorID] = index;
            _nextAbandonedComponent = _actorList[_nextAbandonedComponent];
            _actorList[index] = actorID;
            _abandondedComponentCount--;

            T component(std::forward<Args>(args)...);
            std::swap(component, _componentListPages[index / FF_COMPONENT_MAP_PAGE_SIZE]->at(index % FF_COMPONENT_MAP_PAGE_SIZE));
        }
    }
    template<typename T>
    void ComponentMap<T>::removeComponent(const Actor_t& actor) {
        if(!hasComponent(actor)) {
            return;
        }

        ActorID actorID = convertActorToID(actor);

        Locator::getMessageBus().dispatch<ComponentRemovedEvent<T>>(actor);

        const int& index = _actorIndices[actorID];

        T tempT;
        std::swap(_componentListPages[index / FF_COMPONENT_MAP_PAGE_SIZE]->at(index % FF_COMPONENT_MAP_PAGE_SIZE), tempT);

        _actorList[index] = FF_ACTOR_INVALID;
        if(_abandondedComponentCount > 0) {
            int lastAbandondedComponent = _nextAbandonedComponent;
            while(_actorList[lastAbandondedComponent] != FF_ACTOR_INVALID) {
                lastAbandondedComponent = _actorList[lastAbandondedComponent];
            }
            _actorList[lastAbandondedComponent] = index;
        } else {
            _nextAbandonedComponent = index;
        }
        _abandondedComponentCount++;

        _actorIndices[actorID] = FF_ACTOR_INVALID;
    }
    template<typename T>
    bool ComponentMap<T>::hasComponent(const Actor_t& actor) const {
        return _actorIndices[convertActorToID(actor)] != FF_ACTOR_INVALID;
    }
    template<typename T>
    T& ComponentMap<T>::getComponent(const Actor_t& actor) {
        ActorID actorID = convertActorToID(actor);
        FF_ASSERT(hasComponent(actor), "Actor %s does not have component.", actorID);
        int page = _actorIndices[actorID] / FF_COMPONENT_MAP_PAGE_SIZE;
        return _componentListPages[page]->at(_actorIndices[actorID] % FF_COMPONENT_MAP_PAGE_SIZE);
    }
    template<typename T>
    const T& ComponentMap<T>::getComponent(const Actor_t& actor) const {
        ActorID actorID = convertActorToID(actor);
        FF_ASSERT(hasComponent(actor), "Actor %s does not have component.", actorID);
        int page = _actorIndices[actorID] / FF_COMPONENT_MAP_PAGE_SIZE;
        return _componentListPages[page]->at(_actorIndices[actorID] % FF_COMPONENT_MAP_PAGE_SIZE);
    }
    
    template<typename T>
    void ComponentMap<T>::addNewPage() {
        _componentListPages.emplace_back(std::make_unique<std::array<T, FF_COMPONENT_MAP_PAGE_SIZE>>());
        _actorList.resize(_actorList.size() + FF_COMPONENT_MAP_PAGE_SIZE);
        _actorList.back() = FF_ACTOR_INVALID;
        for(int i = _actorList.size() - FF_COMPONENT_MAP_PAGE_SIZE; i < _actorList.size() - 1; i++) {
            _actorList[i] = i + 1;
        }
        if(_abandondedComponentCount > 0) {
            int lastAbandondedComponent = _nextAbandonedComponent;
            while(_actorList[lastAbandondedComponent] != FF_ACTOR_INVALID) {
                lastAbandondedComponent = _actorList[lastAbandondedComponent];
            }
            _actorList[lastAbandondedComponent] = _actorList.size() - FF_COMPONENT_MAP_PAGE_SIZE;
        } else {
            _nextAbandonedComponent = _actorList.size() - FF_COMPONENT_MAP_PAGE_SIZE;
        }
        _abandondedComponentCount += FF_COMPONENT_MAP_PAGE_SIZE;
    }
}

#endif
