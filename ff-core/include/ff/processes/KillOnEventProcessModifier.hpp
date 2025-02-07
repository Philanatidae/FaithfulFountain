/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_PROCESSES_KILL_ON_EVENT_PROCESS_MANAGER_HPP
#define _FAITHFUL_FOUNTAIN_PROCESSES_KILL_ON_EVENT_PROCESS_MANAGER_HPP

#include <ff/processes/ProcessModifier.hpp>

#include <stdint.h>

#include <type_traits>

#include <ff/messages/Message.hpp>
#include <ff/messages/MessageBus.hpp>
#include <ff/messages/EventListener.hpp>

namespace ff {
    typedef uint8_t KillOnEventFlagsType;

    constexpr KillOnEventFlagsType KILL_ON_EVENT_REGISTER_ON_ATTACH = 0x1;
    constexpr KillOnEventFlagsType KILL_ON_EVENT_END_CHAIN_ON_KILL = 0x2;

    template<typename T>
    class KillOnEventProcessModifier final : public ProcessModifier,
        public ff::EventListener<T> {
    public:
        KillOnEventProcessModifier(const KillOnEventFlagsType& flags = 0);
        virtual ~KillOnEventProcessModifier();

    protected:
        void onAttach();
        void onInitialize();
        void onKill();

        bool processEvent(T const& evt);

    private:
        KillOnEventFlagsType const _flags;

        bool _killOnInitialize;

        void killProcess();
    };

    template<typename T>
    KillOnEventProcessModifier<T>::KillOnEventProcessModifier(const KillOnEventFlagsType& flags)
        :_flags(flags),_killOnInitialize(false) {
    }
    template<typename T>
    KillOnEventProcessModifier<T>::~KillOnEventProcessModifier() {
    }

    template<typename T>
    void KillOnEventProcessModifier<T>::onAttach() {
        if((_flags & KILL_ON_EVENT_REGISTER_ON_ATTACH) == KILL_ON_EVENT_REGISTER_ON_ATTACH) {
            Locator::getMessageBus().addListener<T>(this);
        }
    }
    template<typename T>
    void KillOnEventProcessModifier<T>::onInitialize() {
        if(_killOnInitialize == true) {
            killProcess();
            return;
        }

        if((_flags & KILL_ON_EVENT_REGISTER_ON_ATTACH) != KILL_ON_EVENT_REGISTER_ON_ATTACH) {
            Locator::getMessageBus().addListener<T>(this);
        }
    }
    template<typename T>
    void KillOnEventProcessModifier<T>::onKill() {
        Locator::getMessageBus().removeListener<T>(this);
    }

    template<typename T>//, typename std::enable_if<std::is_base_of<Message, T>::value>::type*>
    void KillOnEventProcessModifier<T>::processEvent(T const& evt) {
        if(getProcess()->getInitialized()) {
            killProcess();
        } else {
            _killOnInitialize = true;
        }
    }

    template<typename T>
    void KillOnEventProcessModifier<T>::killProcess() {
        if((_flags & KILL_ON_EVENT_END_CHAIN_ON_KILL) == KILL_ON_EVENT_END_CHAIN_ON_KILL) {
            getProcess()->setNext(nullptr);
        }
        getProcess()->kill();
    }
}

#endif
