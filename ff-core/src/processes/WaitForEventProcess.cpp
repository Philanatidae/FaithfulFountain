/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/processes/WaitForEventProcess.hpp>

#include <ff/Locator.hpp>

namespace ff {
    WaitForEventProcess::WaitForEventProcess(const std::string& messageName, const bool& registerOnInitialize)
        :WaitForEventProcess({ messageName }, registerOnInitialize) {
    }
    WaitForEventProcess::WaitForEventProcess(std::initializer_list<std::string> messageNames, const bool& registerOnInitialize)
        :WaitForEventProcess(std::vector<std::string>(messageNames), registerOnInitialize) {
    }
    WaitForEventProcess::WaitForEventProcess(const std::vector<std::string>& messageNames, const bool& registerOnInitialize)
        : _messageNames(messageNames), _registerOnInitialize(registerOnInitialize) {
        if (!_registerOnInitialize) {
            registerListeners();
        }
    }
    WaitForEventProcess::~WaitForEventProcess() {
        ff::Locator::getMessageBus().removeListener(this);
    }

    bool WaitForEventProcess::processEvent(const std::shared_ptr<ff::Event>& message) {
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

    void WaitForEventProcess::onInitialize() {
        if (_registerOnInitialize) {
            registerListeners();
        }
    }

    void WaitForEventProcess::registerListeners() {
        for (auto it = _messageNames.begin();
            it != _messageNames.end();
            it++) {
            ff::Locator::getMessageBus().addListener(*it, this);
        }
    }
}
