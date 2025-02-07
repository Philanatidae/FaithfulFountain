/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ACTORS_COMPONENT_HPP
#define _FAITHFUL_FOUNTAIN_ACTORS_COMPONENT_HPP

#include <bitset>
#include <typeindex>

#define FF_MAX_COMPONENTS 64

namespace ff {
    typedef std::bitset<FF_MAX_COMPONENTS> ComponentType;
    constexpr ComponentType FF_ACTOR_COMPONENT_MASK_EMPTY = 0;

    namespace _internal {
        struct IComponent {
            virtual const ComponentType& getComponentType() const = 0;

            virtual ~IComponent() {}
        };
    }

    template<typename T>
    struct Component : public _internal::IComponent {
        virtual ~Component() {
        }

        const ComponentType& getComponentType() const override;
        static const ComponentType& getType();
    };

    namespace _internal {
        ComponentType getNextComponentType();
        ComponentType getComponentType(const std::type_index& type);
    }

    template<typename T>
    const ComponentType& Component<T>::getComponentType() const {
        return Component<T>::getType();
    }
    template<typename T>
    const ComponentType& Component<T>::getType() {
        static ComponentType type = _internal::getComponentType(typeid(Component<T>));
        return type;
    }
}

#endif
