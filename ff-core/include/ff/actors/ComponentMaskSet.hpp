/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ACTORS_COMPONENT_MASK_SET_HPP
#define _FAITHFUL_FOUNTAIN_ACTORS_COMPONENT_MASK_SET_HPP

#include <ff/actors/Actor.hpp>
#include <ff/actors/Component.hpp>
#include <vector>
#include <bitset>

namespace ff {
    class ComponentMaskSet final {
    public:
        ComponentMaskSet();
        ~ComponentMaskSet();

        void addEmptyActor();
        void addEmptyActors(const int& count);

        void addComponent(const Actor_t& actor, const ComponentType& type);
        void removeComponent(const Actor_t& actor, const ComponentType& type);
        bool hasComponent(const Actor_t& actor, const ComponentType& type) const;
        const ComponentType& getComponentMask(const Actor_t& actor) const;

        void clearMask(Actor_t const& actor);

    private:
        std::vector<ComponentType> _componentMasks;
    };
}

#endif
