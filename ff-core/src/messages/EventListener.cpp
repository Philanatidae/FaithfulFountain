/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/messages/EventListener.hpp>
#include <ff/messages/MessageBus.hpp>

namespace ff {

IEventListener::IEventListener()
    :_bus(nullptr) {
}
IEventListener::~IEventListener() {
    if(_bus != nullptr) {
        _bus->implRemoveIEventListener(this);
    }
    _bus = nullptr;
}

bool GenericEventListener::implProcessEvent(std::shared_ptr<Event> const& evt) {
    return processEvent(evt);
}

}
