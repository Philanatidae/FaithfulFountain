/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/processes/WaitOrKillOnEventProcess.hpp>

#include <ff/Locator.hpp>

namespace ff {
    WaitOrKillOnEventProcess::WaitOrKillOnEventProcess(const float& duration, const std::string& messageName, const bool& registerOnInitialize)
        :WaitOrKillOnEventProcess(duration, { messageName }, registerOnInitialize) {
    }
    WaitOrKillOnEventProcess::WaitOrKillOnEventProcess(const float& duration, std::initializer_list<std::string> messageNames, const bool& registerOnInitialize)
        :WaitOrKillOnEventProcess(duration, std::vector<std::string>(messageNames), registerOnInitialize) {
    }
    WaitOrKillOnEventProcess::WaitOrKillOnEventProcess(const float& duration, const std::vector<std::string>& messageNames, const bool& registerOnInitialize)
        :_timer(duration),_messageNames(messageNames),_registerOnInitialize(registerOnInitialize) {
        if (!_registerOnInitialize) {
            registerListeners();
        }
    }
    WaitOrKillOnEventProcess::~WaitOrKillOnEventProcess() {
        ff::Locator::getMessageBus().removeListener(this);
    }

    const Timer& WaitOrKillOnEventProcess::getTimer() const {
        return _timer;
    }
    
    bool WaitOrKillOnEventProcess::processEvent(const std::shared_ptr<ff::Event>& message) {
        for (auto it = _messageNames.begin();
            it != _messageNames.end();
            it++) {
            if ((*it) == message->getName()) {
                kill();
                return false;
            }
        }
        return false;
    }

    void WaitOrKillOnEventProcess::onInitialize() {
        if (_registerOnInitialize) {
            registerListeners();
        }
    }
    void WaitOrKillOnEventProcess::onUpdate(const float& dt) {
        _timer.update(dt);
        if (_timer.hasElapsed()) {
            kill();
        }
    }

    void WaitOrKillOnEventProcess::registerListeners() {
        for (auto it = _messageNames.begin();
            it != _messageNames.end();
            it++) {
            ff::Locator::getMessageBus().addListener(*it, this);
        }
    }
}
