/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_INPUT_GAMEPAD_BUTTON_HPP
#define _FAITHFUL_FOUNTAIN_INPUT_GAMEPAD_BUTTON_HPP

#include <stdint.h>

namespace ff {

using GamepadID_t = int8_t;

using GamepadCapabilities_t = uint16_t;

// NOTE: Least-common denominator for input requirements is the Switch. At
// minimum, controllers must have left-stick, four face-buttons (A/B/X/Y),
// start, left-bumper, right-bumper.

namespace GamepadCapabilities {
    constexpr GamepadCapabilities_t RIGHT_STICK = 1;
    constexpr GamepadCapabilities_t DPAD = 1 << 2;
    constexpr GamepadCapabilities_t TRIGGERS = 1 << 3;
    constexpr GamepadCapabilities_t ANALOG_TRIGGERS = 1 << 4;
    constexpr GamepadCapabilities_t BACK_BUTTON = 1 << 5;
    constexpr GamepadCapabilities_t TOUCHPAD = 1 << 6;
    constexpr GamepadCapabilities_t MOTION = 1 << 7;
    constexpr GamepadCapabilities_t PADDLES_1_AND_2 = 1 << 8;
    constexpr GamepadCapabilities_t PADDLES_2_AND_3 = 1 << 9;
    constexpr GamepadCapabilities_t LEFT_STICK_BUTTON = 1 << 10;
    constexpr GamepadCapabilities_t RIGHT_STICK_BUTTON = 1 << 11;
}

enum class GamepadButton: int {
    UNKNOWN = -1,

    START = 0,
    BACK,

    STICK_LEFT,
    STICK_RIGHT,

    A, // Xbox-style
    B, // Xbox-style
    X, // Xbox-style
    Y, // Xbox-style

    DPAD_LEFT,
    DPAD_RIGHT,
    DPAD_UP,
    DPAD_DOWN,

    BUMPER_LEFT,
    BUMPER_RIGHT,

    TRIGGER_LEFT,
    TRIGGER_RIGHT,

    PADDLE_1,
    PADDLE_2,
    PADDLE_3,
    PADDLE_4,

    TOUCHPAD,

    COUNT
};

enum class GamepadStick : int {
    UNKNOWN = -1,

    LEFT = 0,
    RIGHT,

    COUNT
};
enum class GamepadTrigger : int {
    UNKNOWN = -1,

    LEFT = 0,
    RIGHT,

    COUNT
};

enum class GamepadType: int {
    XBOX,
    DUALSHOCK,
    SWITCH
};

}

#endif
