/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_PROCESS_WAIT_FOR_EVENT_PROCESS_HPP
#define _FAITHFUL_FOUNTAIN_PROCESS_WAIT_FOR_EVENT_PROCESS_HPP

#include <ff/processes/Process.hpp>

#include <ff/messages/EventListener.hpp>

#include <ff/util/Macros.hpp>

#include <vector>
#include <string>

namespace ff {
    class WaitForEventProcess : public Process,
        public GenericEventListener {
    public:
        WaitForEventProcess(const std::string& messageName, const bool& registerOnInitialize = true);
        WaitForEventProcess(std::initializer_list<std::string> messageNames, const bool& registerOnInitialize = true);
        WaitForEventProcess(const std::vector<std::string>& messageNames, const bool& registerOnInitialize = true);
        ~WaitForEventProcess();

        bool processEvent(const std::shared_ptr<ff::Event>& message) override;

    private:
        std::vector<std::string> _messageNames;
        bool _registerOnInitialize;

        void onInitialize() override;

        void registerListeners();
    };
}

#endif
