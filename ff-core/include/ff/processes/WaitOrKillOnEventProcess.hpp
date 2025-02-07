/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_PROCESS_WAIT_OR_KILL_ON_EVENT_PROCESS_HPP
#define _FAITHFUL_FOUNTAIN_PROCESS_WAIT_OR_KILL_ON_EVENT_PROCESS_HPP

#include <ff/processes/Process.hpp>
#include <ff/util/Timer.hpp>

#include <ff/messages/EventListener.hpp>

#include <ff/util/Macros.hpp>

#include <vector>
#include <string>

namespace ff {
    class WaitOrKillOnEventProcess : public Process,
       public GenericEventListener {
    public:
        WaitOrKillOnEventProcess(const float& duration, const std::string& messageName, const bool& registerOnInitialize = true);
        WaitOrKillOnEventProcess(const float& duration, std::initializer_list<std::string> messageNames, const bool& registerOnInitialize = true);
        WaitOrKillOnEventProcess(const float& duration, const std::vector<std::string>& messageNames, const bool& registerOnInitialize = true);
        ~WaitOrKillOnEventProcess();

        const Timer& getTimer() const;

        bool processEvent(const std::shared_ptr<ff::Event>& evt) override;

    private:
        Timer _timer;
        std::vector<std::string> _messageNames;
        bool _registerOnInitialize;

        void onInitialize() override;
        void onUpdate(const float& dt) override;

        void registerListeners();
    };
}

#endif
