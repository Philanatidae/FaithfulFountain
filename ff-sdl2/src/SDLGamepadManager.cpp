/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-sdl2/SDLGamepadManager.hpp>

#include <ff/Console.hpp>
#include <ff/Locator.hpp>
#include <ff/CVars.hpp>

#include <ff/events/input/GamepadConnectedEvent.hpp>
#include <ff/events/input/GamepadDisconnectedEvent.hpp>
#include <ff/events/input/GamepadButtonDownEvent.hpp>
#include <ff/events/input/GamepadButtonUpEvent.hpp>
#include <ff/events/input/GamepadStickChangedEvent.hpp>
#include <ff/events/input/GamepadIDUpdatedEvent.hpp>

#include <cstring>

#include <ff/io/BinaryMemory.hpp>
#include <ff/util/String.hpp>

namespace ff {

void SDLGamepadManager::init() {
    // TODO Needs to be tested
    if(Locator::getEnvironment().getDataStorage().doesConfigExist("gamecontrollerdb.txt")) {
        FF_CONSOLE_LOG("`gamecontrollerdb.txt` detected; loading...");
        auto reader = Locator::getEnvironment().getDataStorage().getConfigReader("gamecontrollerdb.txt");
        BinaryMemory mem(*reader);

        std::istringstream str(mem.toString());
        std::string line;
        int addedMappings = 0;
        int updatedMappings = 0;
        int lineNum = -1;
        while(std::getline(str, line)) {
            lineNum++;
            line = trim(line);
            if(line[0] == '#'
                || line.size() == 0) {
                continue;
            }
            switch(SDL_GameControllerAddMapping(line.c_str())) {
            case 1:
                addedMappings++;
                break;
            case 0:
                updatedMappings++;
                break;
            default:
                FF_CONSOLE_ERROR("Error on Line %i: %s", lineNum, SDL_GetError());
                break;
            }
        }
        FF_CONSOLE_LOG("Added %i mappings, updated %i mappings.", addedMappings, updatedMappings);
    }

    // Initialize events
    Locator::getMessageBus().addHandler<GamepadUpdateIDCmd>(this);
    Locator::getMessageBus().addHandler<ListGamepadsCmd>(this);

    // Set up gamepads that are already connected
    for(int i = 0; i < SDL_NumJoysticks(); i++) {
        addGamepad(i);
    }
}

void SDLGamepadManager::handleEventControllerDevice(SDL_Event const& event) {
    if(event.type == SDL_CONTROLLERDEVICEADDED) {
        addGamepad(event.cdevice.which);
        return;
    }
    if(event.type == SDL_CONTROLLERDEVICEREMOVED) {
        removeGamepad(event.cdevice.which);
        return;
    }
}
void SDLGamepadManager::handleEventControllerButton(SDL_Event const& event) {
    SDL_JoystickID joyID = event.cbutton.which;
    FF_ASSERT(_gamepads.find(joyID) != _gamepads.end(),
        "Gamepad %i not added to manager.",
        joyID);
    SDLGamepadData& gamepadData = _gamepads[joyID];
    SDL_GameControllerButton button = (SDL_GameControllerButton)event.cbutton.button;
    GamepadButton engineButton = convertSDLButtonToGamepadButton(button);
    if(engineButton == GamepadButton::UNKNOWN) {
        return;
    }
    if(event.type == SDL_CONTROLLERBUTTONDOWN) {
        Locator::getMessageBus().dispatch<GamepadButtonDownEvent>(gamepadData.id,
            (uint64_t)joyID,
            engineButton);
        return;
    }
    if(event.type == SDL_CONTROLLERBUTTONUP) {
        Locator::getMessageBus().dispatch<GamepadButtonUpEvent>(gamepadData.id,
            (uint64_t)joyID,
            engineButton);
        return;
    }
}
void SDLGamepadManager::handleEventControllerAxis(SDL_Event const& event) {
    SDL_JoystickID joyID = event.caxis.which;
    SDL_GameControllerAxis axis = (SDL_GameControllerAxis)event.caxis.axis;
    float normalizedAxisVal = (event.caxis.value + 32768) / 65535.0f;
    FF_ASSERT(_gamepads.find(joyID) != _gamepads.end(),
        "Gamepad %i not added to manager.",
        joyID);
    SDLGamepadData& gamepadData = _gamepads[joyID];
    // Two cases; stick, or trigger
    if(axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT) {
        // Triggers are [0, 32767] in non-normalized, so
        // normalized is [0.5, 1].
        bool triggerVal = 2 * (normalizedAxisVal - 0.5f) > CVars::get<float>("input_gamepad_trigger_threshold");
        if(triggerVal && !gamepadData.triggerLeft) {
            Locator::getMessageBus().dispatch<GamepadButtonDownEvent>(gamepadData.id,
                (uint64_t)joyID,
                GamepadButton::TRIGGER_LEFT);
        }
        if(!triggerVal && gamepadData.triggerLeft) {
            Locator::getMessageBus().dispatch<GamepadButtonUpEvent>(gamepadData.id,
                (uint64_t)joyID,
                GamepadButton::TRIGGER_LEFT);
        }
        gamepadData.triggerLeft = triggerVal;
        return;
    }
    if(axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT) {
        bool triggerVal = 2 * (normalizedAxisVal - 0.5f) > CVars::get<float>("input_gamepad_trigger_threshold");
        if(triggerVal && !gamepadData.triggerRight) {
            Locator::getMessageBus().dispatch<GamepadButtonDownEvent>(gamepadData.id,
                (uint64_t)joyID,
                GamepadButton::TRIGGER_RIGHT);
        }
        if(!triggerVal && gamepadData.triggerRight) {
            Locator::getMessageBus().dispatch<GamepadButtonUpEvent>(gamepadData.id,
                (uint64_t)joyID,
                GamepadButton::TRIGGER_RIGHT);
        }
        gamepadData.triggerRight = triggerVal;
        return;
    }

    gamepadData.axes[axis] = normalizedAxisVal;
    GamepadStick engineStick = convertSDLAxisToGamepadStick(axis);

    glm::vec2 const axisVal = engineStick == GamepadStick::LEFT ?
        glm::vec2(gamepadData.axes[SDL_CONTROLLER_AXIS_LEFTX], gamepadData.axes[SDL_CONTROLLER_AXIS_LEFTY]) :
        glm::vec2(gamepadData.axes[SDL_CONTROLLER_AXIS_RIGHTX], gamepadData.axes[SDL_CONTROLLER_AXIS_RIGHTY]);

    Locator::getMessageBus().dispatch<GamepadStickChangedEvent>(gamepadData.id,
        (uint64_t)joyID,
        engineStick,
        glm::vec2(glm::mix(-1.0f, 1.0f, axisVal.x),
            glm::mix(1.0f, -1.0f, axisVal.y)));
}
std::unique_ptr<typename GamepadUpdateIDCmd::Ret> SDLGamepadManager::handleCmd(GamepadUpdateIDCmd const& cmd) {
    // @todo Decide if this should be fatal or non-fatal error
    FF_ASSERT(_gamepads.find(cmd.systemId) != _gamepads.end(), "SDL Game Controller %i does not exist.", cmd.systemId);
    for(auto const& pair : _gamepads) {
        FF_ASSERT(pair.second.id != cmd.id, "Gamepad ID %i already exists.", cmd.id);
    }
    GamepadID_t const prevId = _gamepads[cmd.systemId].id;
    GamepadID_t const newId = _gamepads[cmd.systemId].id = cmd.id;
    Locator::getMessageBus().dispatch<GamepadIDUpdatedEvent>(prevId,
        cmd.systemId,
        newId);

    return std::make_unique<typename GamepadUpdateIDCmd::Ret>();
}
std::unique_ptr<typename ListGamepadsCmd::Ret> SDLGamepadManager::handleCmd(ListGamepadsCmd const& cmd) {
    std::vector<GamepadIDSystemIDPair> gamepads;
    for(auto const& gamepad : _gamepads) {
        gamepads.emplace_back(gamepad.second.id,
            gamepad.first);
    }
    return std::make_unique<typename ListGamepadsCmd::Ret>(gamepads);
}

void SDLGamepadManager::addGamepad(int const& index) {
    FF_CONSOLE_LOG("addGamepad(%s)", index);

    if(!SDL_IsGameController(index)) {
        FF_CONSOLE_WARN("Joystick index %s is not recogized as a gamepad by SDL. Likely, this is an obscure gamepad that is not in the gamepad database. Place a `gamecontrollerdb.txt` with your controller's mapping in the config folder and restart. See `https://github.com/gabomdq/SDL_GameControllerDB` for more information.",
            index);
        return;
    }

    SDL_GameController* controller = SDL_GameControllerOpen(index);
    FF_ASSERT(controller != nullptr, "Could not open SDL_GameController: %s", SDL_GetError());
    SDL_JoystickID joyID = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
    // Seems there is an issue with Series X controllers connecting twice; the ID is the same
    // but the event is generated twice (at least on macOS).
    if(_gamepads.find(joyID) != _gamepads.end()) {
        // Already exists
        SDL_GameControllerClose(controller);
        return;
    }

    SDLGamepadData gamepadData;
    for(int i = 0; i < gamepadData.axes.size(); i++) {
        gamepadData.axes[i] = 0;
    }
    gamepadData.triggerLeft = gamepadData.triggerRight = false;

    gamepadData.controller = controller;

    // SDL does not assign player IDs. On Windows, player index is a thing
    // so we can support that. Technically, PlayerIndex is available on macOS
    // as well, but not reliabily. In the future we will not use SDL for gamepads,
    // getting around this issue.
    gamepadData.id = -1; // To be assigned by the engine
    if((Locator::getEnvironment().getCapabilities() & EnvironmentCapabilities::SYSTEM_GAMEPAD_ASSIGNMENT)
        > 0) {
        gamepadData.id = (GamepadID_t)SDL_GameControllerGetPlayerIndex(controller);
    }
    int playerIndex = SDL_GameControllerGetPlayerIndex(controller);
    gamepadData.name = SDL_GameControllerName(gamepadData.controller);
    FF_CONSOLE_LOG("Detected gamepad %i (SDL %i): `%s`",
        gamepadData.id, joyID, gamepadData.name);
    // This is used by the game to show proper button icons. If a controller
    // type cannot be determined, ff-sdl2 assumes it is an Xbox controller.
    // Note that the Steam Deck has the same buttons as an Xbox controller.
    if(std::strstr(gamepadData.name.c_str(), "PS3")
        || std::strstr(gamepadData.name.c_str(), "PS4")
        || std::strstr(gamepadData.name.c_str(), "PS5")
        || std::strstr(gamepadData.name.c_str(), "PlayStation")
        || std::strstr(gamepadData.name.c_str(), "DualShock")) {
        gamepadData.type = GamepadType::DUALSHOCK;
    } else if(std::strstr(gamepadData.name.c_str(), "Switch")) {
        gamepadData.type = GamepadType::SWITCH;
    } else {
        gamepadData.type = GamepadType::XBOX;
    }
    
    // Determine capabilities
    if(SDL_GameControllerGetBindForButton(gamepadData.controller, SDL_CONTROLLER_BUTTON_LEFTSTICK).bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
        gamepadData.capabilities |= GamepadCapabilities::LEFT_STICK_BUTTON;
    }
    if(SDL_GameControllerGetBindForButton(gamepadData.controller, SDL_CONTROLLER_BUTTON_RIGHTSTICK).bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
        gamepadData.capabilities |= GamepadCapabilities::RIGHT_STICK_BUTTON;
    }
    if(SDL_GameControllerGetBindForButton(gamepadData.controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT).bindType != SDL_CONTROLLER_BINDTYPE_NONE
        || SDL_GameControllerGetBindForButton(gamepadData.controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT).bindType != SDL_CONTROLLER_BINDTYPE_NONE
        || SDL_GameControllerGetBindForButton(gamepadData.controller, SDL_CONTROLLER_BUTTON_DPAD_UP).bindType != SDL_CONTROLLER_BINDTYPE_NONE
        || SDL_GameControllerGetBindForButton(gamepadData.controller, SDL_CONTROLLER_BUTTON_DPAD_DOWN).bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
        gamepadData.capabilities |= GamepadCapabilities::DPAD;
    }
    if(SDL_GameControllerGetBindForButton(gamepadData.controller, SDL_CONTROLLER_BUTTON_BACK).bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
        gamepadData.capabilities |= GamepadCapabilities::BACK_BUTTON;
    }

    if(SDL_GameControllerGetBindForAxis(gamepadData.controller, SDL_CONTROLLER_AXIS_RIGHTX).bindType != SDL_CONTROLLER_BINDTYPE_NONE
        && SDL_GameControllerGetBindForAxis(gamepadData.controller, SDL_CONTROLLER_AXIS_RIGHTY).bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
        gamepadData.capabilities |= GamepadCapabilities::RIGHT_STICK;
    }

    // TODO Support analog triggers. This wasn't supported in GLFW because of implementation limitations. Somehow we need to figure out how to differentiate between binary triggers and analog triggers.
    if(SDL_GameControllerGetBindForButton(gamepadData.controller, SDL_CONTROLLER_BUTTON_BACK).bindType != SDL_CONTROLLER_BINDTYPE_NONE) {
        gamepadData.capabilities |= GamepadCapabilities::TRIGGERS;
    }

    // TODO Support paddles
    // TODO Support touchpad
    // TODO Support motion
    
    _gamepads.emplace(joyID, gamepadData);
    
    Locator::getMessageBus().dispatch<GamepadConnectedEvent>(gamepadData.id,
        joyID,
        gamepadData.name,
        gamepadData.type,
        gamepadData.capabilities);
}
void SDLGamepadManager::removeGamepad(SDL_JoystickID const& id) {
    if(_gamepads.find(id) == _gamepads.end()) {
        return;
    }
    SDLGamepadData& gamepadData = _gamepads[id];
    Locator::getMessageBus().dispatch<GamepadDisconnectedEvent>(gamepadData.id, id);
    SDL_GameControllerClose(gamepadData.controller);
    gamepadData.controller = nullptr;
    _gamepads.erase(id);
}

GamepadButton SDLGamepadManager::convertSDLButtonToGamepadButton(SDL_GameControllerButton const& button) {
    switch(button) {
    case SDL_CONTROLLER_BUTTON_A:
        return GamepadButton::A;
    case SDL_CONTROLLER_BUTTON_B:
        return GamepadButton::B;
    case SDL_CONTROLLER_BUTTON_X:
        return GamepadButton::X;
    case SDL_CONTROLLER_BUTTON_Y:
        return GamepadButton::Y;

    case SDL_CONTROLLER_BUTTON_BACK:
        return GamepadButton::BACK;
    case SDL_CONTROLLER_BUTTON_START:
        return GamepadButton::START;

    case SDL_CONTROLLER_BUTTON_LEFTSTICK:
        return GamepadButton::STICK_LEFT;
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
        return GamepadButton::STICK_RIGHT;
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
        return GamepadButton::BUMPER_LEFT;
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
        return GamepadButton::BUMPER_RIGHT;

    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        return GamepadButton::DPAD_UP;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        return GamepadButton::DPAD_DOWN;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        return GamepadButton::DPAD_LEFT;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        return GamepadButton::DPAD_RIGHT;

    case SDL_CONTROLLER_BUTTON_PADDLE1:
        return GamepadButton::PADDLE_1;
    case SDL_CONTROLLER_BUTTON_PADDLE2:
        return GamepadButton::PADDLE_2;
    case SDL_CONTROLLER_BUTTON_PADDLE3:
        return GamepadButton::PADDLE_3;
    case SDL_CONTROLLER_BUTTON_PADDLE4:
        return GamepadButton::PADDLE_4;

    case SDL_CONTROLLER_BUTTON_TOUCHPAD:
        return GamepadButton::TOUCHPAD;

    default:
        return GamepadButton::UNKNOWN;
    }
}
GamepadStick SDLGamepadManager::convertSDLAxisToGamepadStick(SDL_GameControllerAxis const& axis) {
    switch(axis) {
    case SDL_CONTROLLER_AXIS_LEFTX:
    case SDL_CONTROLLER_AXIS_LEFTY:
        return GamepadStick::LEFT;
    case SDL_CONTROLLER_AXIS_RIGHTX:
    case SDL_CONTROLLER_AXIS_RIGHTY:
        return GamepadStick::RIGHT;
    default:
        return GamepadStick::UNKNOWN;
    }
}

}

