/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_EVENTS_ACTORS_COMPONENT_REMOVED_EVENT_HPP
#define _FAITHFUL_FOUNTAIN_EVENTS_ACTORS_COMPONENT_REMOVED_EVENT_HPP

#include <ff/messages/Event.hpp>

#include <ff/actors/Actor.hpp>

#include <tinyformat/tinyformat.h>
#include <typeinfo>

namespace ff {

template<typename T>
class ComponentRemovedEvent: public ff::Event {
public:
    ComponentRemovedEvent(Actor_t const& actor)
        :actor(actor) { }
    ~ComponentRemovedEvent() = default;

    static char const* getEventName() {
        static std::string name = tinyformat::format("evt_component_removed_%s", typeid(T).name()).c_str();
        return name.c_str();
    }

    inline std::string getName() const override {
        return getEventName();
    };
    inline std::string stringify() const override {
        return tinyformat::format("%s from actor: %s", typeid(T).name(), actor);
    };

    Actor_t actor; 
};

}

#endif
