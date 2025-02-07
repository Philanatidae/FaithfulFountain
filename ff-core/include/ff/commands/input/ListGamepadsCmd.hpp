/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_COMMANDS_INPUT_LIST_GAMEPADS_COMMAND_HPP
#define _FF_COMMANDS_INPUT_LIST_GAMEPADS_COMMAND_HPP

#include <vector>
#include <utility>

#include <ff/input/Gamepad.hpp>

#include <ff/messages/CmdHelpers.hpp>

namespace ff {
    struct GamepadIDSystemIDPair {
        GamepadIDSystemIDPair(GamepadID_t const& gamepadId,
            uint64_t const& systemId)
            :gamepadId(gamepadId),
            systemId(systemId) {
        }

        GamepadID_t gamepadId;
        uint64_t systemId;
    };

    inline std::string stringifyGamepads(const std::vector<GamepadIDSystemIDPair>& gamepads) {
        std::string result = "[";
        for (size_t i = 0; i < gamepads.size(); ++i) {
            if (i > 0) {
                result += ", ";
            }
            result += tfm::format("(%d, %llu)", gamepads[i].gamepadId, gamepads[i].systemId);
        }
        result += "]";
        return result;
    }

    FF_CMD_DEFINE_0_R1(ListGamepadsCmd,
        "cmd_list_gamepads",
        "List all connected gamepads",
        std::vector<GamepadIDSystemIDPair>, gamepads,
        "%s",
        (stringifyGamepads(gamepads)));    
}

#endif

