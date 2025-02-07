/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/messages/MessageBus.hpp>

#include <regex>
#include <ff/CVars.hpp>

namespace ff {

MessageBus::MessageBus()
    :_isProcessing(false) {
}
MessageBus::~MessageBus() {
    for(auto& pair : _eventListeners) {
        for(auto& listener : pair.second) {
            listener->_bus = nullptr;
        }
    }
    _eventListeners.clear();
    _eventListenersPendingAdd.clear();
    _eventListenersPendingRemove.clear();

    for(auto& handlerPair : _cmdHandlers) {
        if(handlerPair.second == nullptr) {
            continue;
        }
        handlerPair.second->_bus = nullptr;
        handlerPair.second = nullptr;
    }
}

void MessageBus::addListener(const std::string& eventName,
    GenericEventListener* const& listenerPtr,
    MessageListenerPriority_t const& priority) { 
    std::vector<IEventListener*>& eventListeners = _eventListeners[eventName];
    IEventListener* iListenerPtr = (IEventListener*)listenerPtr;
    if(std::find(eventListeners.begin(),
        eventListeners.end(),
        iListenerPtr)
        != eventListeners.end()) {
        return;
    }
    iListenerPtr->_priority = priority;
    
    if(_isProcessing) {
        _eventListenersPendingAdd[eventName].push_back(iListenerPtr);
        return;
    }

    iListenerPtr->_bus = this;
    eventListeners.push_back(iListenerPtr);
    std::sort(eventListeners.begin(),
        eventListeners.end(),
        [](IEventListener* lhs, IEventListener* rhs) -> bool {
        return lhs->_priority > rhs->_priority;
    });
}
void MessageBus::removeListener(const std::string& eventName, GenericEventListener* const& listenerPtr) {
    std::vector<IEventListener*>& eventListeners = _eventListeners[eventName];
    IEventListener* iListenerPtr = (IEventListener*)listenerPtr;

    if(_isProcessing) {
        _eventListenersPendingRemove[eventName].push_back(iListenerPtr);
        return;
    }

    if(std::find(eventListeners.begin(),
        eventListeners.end(),
        iListenerPtr)
        != eventListeners.end()) {
        eventListeners.erase(std::remove(eventListeners.begin(),
            eventListeners.end(),
            iListenerPtr),
            eventListeners.end());
    }
}
void MessageBus::removeListener(GenericEventListener* const& listenerPtr) {
    implRemoveIEventListener(listenerPtr);
}

void MessageBus::implRemoveIEventListener(IEventListener* listener) {
    if(_isProcessing) {
        _eventListenersPendingAllRemove.push_back(listener);
        return;
    }
    for(auto& pair : _eventListeners) {
        pair.second.erase(std::remove(pair.second.begin(),
            pair.second.end(),
            listener),
            pair.second.end());
    }
    listener->_bus = nullptr;
}
void MessageBus::implRemoveICmdHandler(ICmdHandler* handler) {
    for(auto& handlerPair : _cmdHandlers) {
        if(handlerPair.second != handler) {
            continue;
        }
        handler->_bus = nullptr;
        handlerPair.second = nullptr;
    }
}

void MessageBus::cleanupListenersAndHandlers() {
    FF_ASSERT(!_isProcessing, "MessageBus is processing; cannot clean up.");

    // Cleanup listeners
    for(auto& pair : _eventListenersPendingRemove) {
        for(auto& listener : pair.second) {
            _eventListeners[pair.first].erase(std::remove(_eventListeners[pair.first].begin(),
                _eventListeners[pair.first].end(),
                listener),
                _eventListeners[pair.first].end());
        }
    }
    _eventListenersPendingRemove.clear();
    for(auto& listener : _eventListenersPendingAllRemove) {
        implRemoveIEventListener(listener);
    }
    _eventListenersPendingAllRemove.clear();

    for(auto& pair : _eventListenersPendingAdd) {
        for(auto& listener : pair.second) {
            listener->_bus = this;
            _eventListeners[pair.first].push_back(listener);
        }
        std::sort(_eventListeners[pair.first].begin(),
            _eventListeners[pair.first].end(),
            [](IEventListener* lhs, IEventListener* rhs) -> bool {
            return lhs->_priority > rhs->_priority;
        });
    }
    _eventListenersPendingAdd.clear();

    // Handlers don't need cleaned up; only one handler per cmd
}

}
