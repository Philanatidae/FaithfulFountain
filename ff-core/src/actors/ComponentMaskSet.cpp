/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/actors/ComponentMaskSet.hpp>

namespace ff {
    ComponentMaskSet::ComponentMaskSet() {
    }
    ComponentMaskSet::~ComponentMaskSet() {
    }

    void ComponentMaskSet::addEmptyActor() {
        _componentMasks.push_back(0);
    }
    void ComponentMaskSet::addEmptyActors(const int& count) {
        for(int i = 0; i < count; ++i) {
            _componentMasks.push_back(0);
        }
    }

    void ComponentMaskSet::addComponent(const Actor_t& actor, const ComponentType& type) {
        _componentMasks[convertActorToID(actor)] |= type;
    }
    void ComponentMaskSet::removeComponent(const Actor_t& actor, const ComponentType& type) {
        _componentMasks[convertActorToID(actor)] &= ~type;
    }
    bool ComponentMaskSet::hasComponent(const Actor_t& actor, const ComponentType& type) const {
        return (_componentMasks[convertActorToID(actor)] & type) != 0;
    }
    const ComponentType& ComponentMaskSet::getComponentMask(const Actor_t& actor) const {
        return _componentMasks[convertActorToID(actor)];
    }

    void ComponentMaskSet::clearMask(Actor_t const& actor) {
        _componentMasks[convertActorToID(actor)] = 0;
    }
}
