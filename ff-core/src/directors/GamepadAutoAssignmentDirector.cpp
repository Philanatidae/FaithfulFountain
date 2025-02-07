/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/directors/GamepadAutoAssignmentDirector.hpp>

#include <ff/Locator.hpp>

#include <ff/commands/input/GamepadUpdateIDCmd.hpp>

#include <glm/glm.hpp>

namespace ff {

GamepadAutoAssignmentDirector::GamepadAutoAssignmentDirector(ProcessPriority_t const& priority)
    :Process(priority),
    _maxID(-1) {
}

bool GamepadAutoAssignmentDirector::processEvent(GamepadConnectedEvent const& evt) {
    if(evt.id > -1) {
        _maxID = glm::max(_maxID, evt.id);
        auto it = _freeIDs.find(evt.id);
        if(it != _freeIDs.end()) {
            _freeIDs.erase(it);
        }
        return false;
    }

    GamepadID_t newId = -1;
    if(_freeIDs.size() > 0) {
        newId = *_freeIDs.begin();
        _freeIDs.erase(_freeIDs.begin());
    } else {
        newId = ++_maxID;
    }
    Locator::getMessageBus().dispatch<GamepadUpdateIDCmd>(evt.systemID,
        newId);
    return false;
}
bool GamepadAutoAssignmentDirector::processEvent(GamepadDisconnectedEvent const& evt) {
    if(evt.id > -1) {
        _freeIDs.insert(evt.id);
    }

    return false;
}
bool GamepadAutoAssignmentDirector::processEvent(GamepadIDUpdatedEvent const& evt) {
    _maxID = glm::max(_maxID, evt.id);
    auto it = _freeIDs.find(evt.id);
    if(it != _freeIDs.end()) {
        _freeIDs.erase(it);
    }
    
    if(evt.prevId > -1) {
        _freeIDs.insert(evt.prevId);
    }

    return false;
}

void GamepadAutoAssignmentDirector::onInitialize() {
    Locator::getMessageBus().addListener<GamepadConnectedEvent>(this);
    Locator::getMessageBus().addListener<GamepadDisconnectedEvent>(this);
    Locator::getMessageBus().addListener<GamepadIDUpdatedEvent>(this);
}

}

