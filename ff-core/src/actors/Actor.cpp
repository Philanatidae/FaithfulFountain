/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

// #include <ff/actors/Actor.hpp>

// namespace ff {
//     Actor::Actor(ActorManager* const& actorManager)
//         :_actorManagerPtr(actorManager) {
//     }
//     Actor::~Actor() {
//         clean();
//     }

//     ActorManager* const& Actor::getActorManager() const {
//         return _actorManagerPtr;
//     }
//     bool Actor::isValid() const {
//         if(_actorManagerPtr == nullptr) {
//             return false;
//         }
//         return _actorManagerPtr != nullptr;
//     }

//     void Actor::stripAllExcept(const ComponentType& mask) {
//         ComponentType prevMask = _componentMask;
//         ComponentType stripMask;
//         size_t idx = 0;
//         while((stripMask = (_componentMask & (~mask))) != 0) {
//             while(stripMask[idx] == 0) {
//                 ++idx;
//             }
//             ComponentType removeType(0);
//             removeType[idx] = 1;

//             _internal::IComponent* compPtr = _componentMap[removeType];
//             _componentMap.erase(removeType);
//             delete compPtr;
//             _componentMask &= (~removeType);
//         }

//         _actorManagerPtr->updateFamilyActorMapForActor(shared_from_this(), prevMask);
//     }

//     void Actor::clean() {
//         for(auto it = _componentMap.begin();
//             it != _componentMap.end();
//             it++) {
//             _internal::IComponent* compPtr = it->second;
//             delete compPtr;
//         }
//         _componentMap.clear();
//         _componentMask = 0;
//     }

//     ComponentType Actor::getComponentMask() const {
//         return _componentMask;
//     }
// }
