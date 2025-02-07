/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_DIRECTORS_GAMEPAD_AUTO_ASSIGNMENT_DIRECTOR_HPP
#define _FAITHFUL_FOUNTAIN_DIRECTORS_GAMEPAD_AUTO_ASSIGNMENT_DIRECTOR_HPP

#include <ff/processes/Process.hpp>

#include <ff/messages/EventListener.hpp>
#include <ff/events/input/GamepadConnectedEvent.hpp>
#include <ff/events/input/GamepadDisconnectedEvent.hpp>
#include <ff/events/input/GamepadIDUpdatedEvent.hpp>

#include <ff/input/Gamepad.hpp>
#include <set>

namespace ff {

class GamepadAutoAssignmentDirector final : public Process,
    EventListener<GamepadConnectedEvent>,
    EventListener<GamepadDisconnectedEvent>,
    EventListener<GamepadIDUpdatedEvent> {
public:
    GamepadAutoAssignmentDirector(ProcessPriority_t const& priority = ProcessPriority::DEFAULT);
    ~GamepadAutoAssignmentDirector() = default;

protected:
    bool processEvent(GamepadConnectedEvent const& evt) override;
    bool processEvent(GamepadDisconnectedEvent const& evt) override;
    bool processEvent(GamepadIDUpdatedEvent const& evt) override;

    void onInitialize() override;

private:
    GamepadID_t _maxID;
    std::set<GamepadID_t> _freeIDs;
};

}

#endif

