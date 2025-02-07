/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_MESSAGES_MESSAGE_BUS_HPP
#define _FAITHFUL_FOUNTAIN_MESSAGES_MESSAGE_BUS_HPP

#include <ff/messages/Event.hpp>
#include <ff/messages/EventListener.hpp>
#include <ff/messages/Cmd.hpp>
#include <ff/messages/CmdHandler.hpp>
#include <ff/messages/MessageListenerPriority.hpp>

#include <ff/Console.hpp>
#include <ff/CVars.hpp>
#include <ff/io/Serializer.hpp>

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <regex>

namespace ff {

constexpr char const* MESSAGE_WILDCARD = "*";

class MessageBus final {
friend class IEventListener;
friend class ICmdHandler;

public:
    MessageBus();
    ~MessageBus();

    template<typename T, typename... Args, typename std::enable_if<std::is_base_of<Event, T>::value>::type* En = nullptr>
    void dispatch(Args... args);
    template<typename T, typename... Args, typename std::enable_if<std::is_base_of<Cmd<typename T::Ret>, T>::value>::type* En = nullptr>
    std::unique_ptr<typename T::Ret> dispatch(Serializer& serializer);
    template<typename T, typename... Args, typename std::enable_if<std::is_base_of<Cmd<typename T::Ret>, T>::value>::type* En = nullptr>
    std::unique_ptr<typename T::Ret> dispatch(Args... args);

    void addListener(const std::string& eventName,
        GenericEventListener* const& listenerPtr,
        MessageListenerPriority_t const& priority = MessageListenerPriority::DEFAULT);
    void removeListener(const std::string& eventName, GenericEventListener* const& listenerPtr);
    void removeListener(GenericEventListener* const& listenerPtr);

    template<typename T, typename std::enable_if<std::is_base_of<Event, T>::value>::type* En = nullptr>
    void addListener(EventListener<T>* const& listenerPtr,
        MessageListenerPriority_t const& priority = MessageListenerPriority::DEFAULT);
    template<typename T, typename std::enable_if<std::is_base_of<Event, T>::value>::type* En = nullptr>
    void removeListener(EventListener<T>* const& listenerPtr);

    template<typename T, typename std::enable_if<std::is_base_of<Cmd<typename T::Ret>, T>::value>::type* En = nullptr>
    void addHandler(CmdHandler<T, En>* const& handlerPtr);
    template<typename T, typename std::enable_if<std::is_base_of<Cmd<typename T::Ret>, T>::value>::type* En = nullptr>
    void removeHandler(CmdHandler<T, En>* const& handlerPtr);

private:
    std::unordered_map<std::string, std::vector<IEventListener*>> _eventListeners;
    std::unordered_map<std::string, std::vector<IEventListener*>> _eventListenersPendingAdd;
    std::unordered_map<std::string, std::vector<IEventListener*>> _eventListenersPendingRemove;
    std::vector<IEventListener*> _eventListenersPendingAllRemove;
    std::unordered_map<std::string, ICmdHandler*> _cmdHandlers;
    bool _isProcessing;

    void implRemoveIEventListener(IEventListener* listener);
    void implRemoveICmdHandler(ICmdHandler* handler);

    void cleanupListenersAndHandlers();
};
}

namespace ff {
template<typename T, typename... Args, typename std::enable_if<std::is_base_of<Event, T>::value>::type* En>
void MessageBus::dispatch(Args... args) { // Event dispatch
    auto evt = std::make_shared<T>(args...);

    _isProcessing = true;

    if(!std::regex_match(evt->getName(), std::regex(CVars::get<std::string>("message_filter")))) { // Exclusive
        FF_CONSOLE_LOG("[%s] %s", evt->getName(), evt->stringify());
    }

    std::vector<IEventListener*>& wildcardListeners = _eventListeners[MESSAGE_WILDCARD];
    std::vector<IEventListener*>& listenersPendingRemoval = _eventListenersPendingRemove[MESSAGE_WILDCARD];
    for(int i = 0; i < wildcardListeners.size(); i++) {
        if(std::find(_eventListenersPendingRemove[MESSAGE_WILDCARD].begin(),
            _eventListenersPendingRemove[MESSAGE_WILDCARD].end(),
            wildcardListeners[i])
            != _eventListenersPendingRemove[MESSAGE_WILDCARD].end()) {
            continue;
        }
        if (wildcardListeners[i]->implProcessEvent(evt)) {
            return;
        }
    }
    std::string const name = evt->getName();
    std::vector<IEventListener*>& eventListeners = _eventListeners[name];
    listenersPendingRemoval = _eventListenersPendingRemove[name];
    for(int i = 0; i < eventListeners.size(); i++) {
        if(std::find(listenersPendingRemoval.begin(),
            listenersPendingRemoval.end(),
            eventListeners[i])
            != listenersPendingRemoval.end()) {
            continue;
        }
        if(eventListeners[i]->implProcessEvent(evt)) {
            return;
        }
    }

    _isProcessing = false;

    cleanupListenersAndHandlers();
}
template<typename T, typename... Args, typename std::enable_if<std::is_base_of<Cmd<typename T::Ret>, T>::value>::type* En>
std::unique_ptr<typename T::Ret> MessageBus::dispatch(Args... args) { // Cmd dispatch                                                                  //
    T cmd(args...);

    auto it = _cmdHandlers.find(cmd.getName());
    if(!std::regex_match(cmd.getName(), std::regex(CVars::get<std::string>("message_filter")))) { // Exclusive
        FF_CONSOLE_LOG("[%s] (%s) %s", cmd.getName(), it != _cmdHandlers.end() ? "ACK" : "NACK", cmd.stringify());
    }
    if(it == _cmdHandlers.end()
        || it->second == nullptr) {
        return nullptr;
    }

    CmdHandler<T>* handler = static_cast<CmdHandler<T>*>(it->second);
    return handler->handleCmd(cmd);
}
template<typename T, typename... Args, typename std::enable_if<std::is_base_of<Cmd<typename T::Ret>, T>::value>::type* En>
std::unique_ptr<typename T::Ret> MessageBus::dispatch(Serializer& serializer) {
    FF_ASSERT(serializer.getDirection() == ff::SerializerDirection::READ,
        "Serializer direction must be READ.");
    T cmd;
    cmd.serialize(serializer);

    auto it = _cmdHandlers.find(cmd.getName());
    if(!std::regex_match(cmd.getName(), std::regex(CVars::get<std::string>("message_filter")))) { // Exclusive
        FF_CONSOLE_LOG("[%s] (%s) %s", cmd.getName(), it != _cmdHandlers.end() ? "ACK" : "NACK", cmd.stringify());
    }
    if(it == _cmdHandlers.end()
        || it->second == nullptr) {
        return nullptr;
    }

    CmdHandler<T>* handler = static_cast<CmdHandler<T>*>(it->second);
    return handler->handleCmd(cmd);
}

template<typename T, typename std::enable_if<std::is_base_of<Event, T>::value>::type* En>
void MessageBus::addListener(EventListener<T>* const& listenerPtr,
    MessageListenerPriority_t const& priority) {
    std::vector<IEventListener*>& eventListeners = _eventListeners[T::getEventName()];
    IEventListener* iListenerPtr = (IEventListener*)listenerPtr;
    if(std::find(eventListeners.begin(),
        eventListeners.end(),
        iListenerPtr)
        != eventListeners.end()) {
        return;
    }
    iListenerPtr->_priority = priority;
    
    if(_isProcessing) {
        _eventListenersPendingAdd[T::getEventName()].push_back(iListenerPtr);
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
template<typename T, typename std::enable_if<std::is_base_of<Event, T>::value>::type* En>
void MessageBus::removeListener(EventListener<T>* const& listenerPtr) {
    std::vector<IEventListener*>& eventListeners = _eventListeners[T::getEventName()];
    IEventListener* iListenerPtr = (IEventListener*)listenerPtr;

    if(_isProcessing) {
        _eventListenersPendingRemove[T::getEventName()].push_back(iListenerPtr);
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

template<typename T, typename std::enable_if<std::is_base_of<Cmd<typename T::Ret>, T>::value>::type* En>
void MessageBus::addHandler(CmdHandler<T, En>* const& handlerPtr) {
    FF_ASSERT(_cmdHandlers.find(T::getCmdName()) == _cmdHandlers.end()
        || _cmdHandlers[T::getCmdName()] == nullptr,
        "Handler already registered for `%s`.", T::getCmdName());
    _cmdHandlers[T::getCmdName()] = handlerPtr;
    handlerPtr->_bus = this;
}
template<typename T, typename std::enable_if<std::is_base_of<Cmd<typename T::Ret>, T>::value>::type* En>
void MessageBus::removeHandler(CmdHandler<T, En>* const& handlerPtr) {
    auto it = _cmdHandlers.find(T::getCmdName());
    if(it == _cmdHandlers.end()) {
        return;
    }

    FF_ASSERT(it->second == handlerPtr, "`%s` is registered to a different handler.");
    handlerPtr->_bus = nullptr;
    it->second = nullptr;
}

}

#endif
