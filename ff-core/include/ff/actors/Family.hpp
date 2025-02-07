/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ACTORS_FAMILY_HPP
#define _FAITHFUL_FOUNTAIN_ACTORS_FAMILY_HPP

#include <ff/actors/Component.hpp>
#include <ff/util/Hash.hpp>
#include <unordered_set>
#include <memory>
#include <ff/actors/Actor.hpp>

namespace ff {
    class FamilyBuilder;
    class ActorManager;

    class Family final {
    friend class FamilyBuilder;
    friend struct std::hash<Family>;

    public:
        Family(const Family& other);
        virtual ~Family();

        bool matches(ActorManager* const& actorManager, const Actor_t& actor) const;
        bool matches(const ComponentType& mask) const;

        bool equals(const Family& other) const;
        bool operator==(const Family& rhs) const;
        bool operator!=(const Family& rhs) const;

        template<typename... Components>
        static FamilyBuilder all();
        template<typename... Components>
        static FamilyBuilder one();
        template<typename... Components>
        static FamilyBuilder exclude();
    private:
        Family();

        ComponentType _allMask;
        ComponentType _oneMask;
        ComponentType _excludeMask;
    };

    class FamilyBuilder {
    friend class Family;
    public:
        virtual ~FamilyBuilder();

        template<typename ... Components>
        FamilyBuilder& all();
        template<typename ... Components>
        FamilyBuilder& one();
        template<typename ... Components>
        FamilyBuilder& exclude();
        Family get();

    private:
        FamilyBuilder();
        Family _family;
    };
}
namespace std {
    template<>
    struct hash<ff::Family> {
        size_t operator()(const ff::Family& family) const {
            size_t seed(0);
            ff::hash_combine(seed, family._allMask);
            ff::hash_combine(seed, family._oneMask);
            ff::hash_combine(seed, family._excludeMask);
            return seed;
        }
    };
}

#include <ff/actors/ActorManager.hpp>
#include <ff/util/Conjunction.hpp>

namespace ff {

    template <typename... Components>
    FamilyBuilder Family::all() {
        static_assert(conjunction<std::integral_constant<bool, std::is_base_of<_internal::IComponent, Components>::value>...>::value,
            "Component does not extend off of Component<T>.");

        FamilyBuilder familyBuilder;
        familyBuilder.all<Components...>();
        return familyBuilder;
    }
    template <typename... Components>
    FamilyBuilder Family::one() {
        static_assert(conjunction<std::integral_constant<bool, std::is_base_of<_internal::IComponent, Components>::value>...>::value,
            "Component does not extend off of Component<T>.");

        FamilyBuilder familyBuilder;
        familyBuilder.one<Components...>();
        return familyBuilder;
    }
    template <typename... Components>
    FamilyBuilder Family::exclude() {
        static_assert(conjunction<std::integral_constant<bool, std::is_base_of<_internal::IComponent, Components>::value>...>::value,
            "Component does not extend off of Component<T>.");

        FamilyBuilder familyBuilder;
        familyBuilder.exclude<Components...>();
        return familyBuilder;
    }

    template <typename... Components>
    FamilyBuilder& FamilyBuilder::all() {
        static_assert(conjunction<std::integral_constant<bool, std::is_base_of<_internal::IComponent, Components>::value>...>::value,
            "Component does not extend off of Component<T>.");

        std::unordered_set<ComponentType> types = { Component<Components>::getType()... };
        for(auto it = types.begin();
            it != types.end();
            it++) {
            _family._allMask |= *it;
        }

        return *this;
    }
    template <typename... Components>
    FamilyBuilder& FamilyBuilder::one() {
        static_assert(conjunction<std::integral_constant<bool, std::is_base_of<_internal::IComponent, Components>::value>...>::value,
            "Component does not extend off of Component<T>.");

        std::unordered_set<ComponentType> types = { Component<Components>::getType()... };

        for(auto it = types.begin();
            it != types.end();
            it++) {
            _family._oneMask |= *it;
        }

        return *this;
    }
    template <typename... Components>
    FamilyBuilder& FamilyBuilder::exclude() {
        static_assert(conjunction<std::integral_constant<bool, std::is_base_of<_internal::IComponent, Components>::value>...>::value,
            "Component does not extend off of Component<T>.");

        std::unordered_set<ComponentType> types = { Component<Components>::getType()... };

        for(auto it = types.begin();
            it != types.end();
            it++) {
            _family._excludeMask |= *it;
        }

        return *this;
    }
}

#endif
