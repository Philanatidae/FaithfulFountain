/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_MESSAGES_EVENT_LISTENER_HPP
#define _FAITHFUL_FOUNTAIN_MESSAGES_EVENT_LISTENER_HPP

#include <ff/messages/Event.hpp>
#include <ff/messages/MessageListenerPriority.hpp>

#include <memory>

namespace ff {

class MessageBus;

class IEventListener {
friend class MessageBus;
public:
    IEventListener();
    virtual ~IEventListener();
private:
    MessageBus* _bus;
    MessageListenerPriority_t _priority;

    virtual bool implProcessEvent(std::shared_ptr<Event> const& evt) = 0;
};

class GenericEventListener : public IEventListener {
friend class MessageBus;
public:
    virtual bool processEvent(std::shared_ptr<Event> const& evt) = 0;

private:
    bool implProcessEvent(std::shared_ptr<Event> const& evt) override;
};

template<typename T, typename std::enable_if<std::is_base_of<Event, T>::value>::type* En = nullptr>
class EventListener : public IEventListener {
friend class MessageBus;
public:
    virtual bool processEvent(T const& evt) = 0;

private:
    bool implProcessEvent(std::shared_ptr<Event> const& evt) override;
};

template<typename T, typename std::enable_if<std::is_base_of<Event, T>::value>::type* En>
bool EventListener<T, En>::implProcessEvent(std::shared_ptr<Event> const& evt) {
    T* tEvt = static_cast<T*>(evt.get());
    return processEvent(*tEvt);
}

}

#endif
