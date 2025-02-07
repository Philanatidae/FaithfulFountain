/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_SDL_GAMEPAD_MANAGER_HPP
#define _FF_SDL_GAMEPAD_MANAGER_HPP

#include <SDL.h>

#include <ff/input/Gamepad.hpp>

#include <array>
#include <string>

#include <unordered_map>

#include <ff/messages/CmdHandler.hpp>
#include <ff/commands/input/GamepadUpdateIDCmd.hpp>
#include <ff/commands/input/ListGamepadsCmd.hpp>

namespace ff {

struct SDLGamepadData {
    GamepadID_t id;
    std::string name;
    GamepadType type;

    GamepadCapabilities_t capabilities;

    std::array<float, SDL_CONTROLLER_AXIS_MAX> axes;

    SDL_GameController* controller;

    bool triggerLeft;
    bool triggerRight;
};

class SDLGamepadManager final
    : public CmdHandler<GamepadUpdateIDCmd>,
    public CmdHandler<ListGamepadsCmd> {
public:
    ~SDLGamepadManager() = default;

    void init();

    void handleEventControllerDevice(SDL_Event const& event);
    void handleEventControllerButton(SDL_Event const& event);
    void handleEventControllerAxis(SDL_Event const& event);

protected:
    std::unique_ptr<typename GamepadUpdateIDCmd::Ret> handleCmd(GamepadUpdateIDCmd const& cmd) override;
    std::unique_ptr<typename ListGamepadsCmd::Ret> handleCmd(ListGamepadsCmd const& cmd) override;

private:
    std::unordered_map<SDL_JoystickID, SDLGamepadData> _gamepads;

    void addGamepad(int const& index);
    void removeGamepad(SDL_JoystickID const& id);

    GamepadButton convertSDLButtonToGamepadButton(SDL_GameControllerButton const& button);
    GamepadStick convertSDLAxisToGamepadStick(SDL_GameControllerAxis const& axis);
};

}

#endif

