/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_INPUT_KEYBOARD_HPP
#define _FAITHFUL_FOUNTAIN_INPUT_KEYBOARD_HPP

#include <stdint.h>

namespace ff {
    enum class KeyboardKey: int {
        UNKNOWN = -1,

        SPACE = 0,
        APOSTROPHE,
        COMMA,
        MINUS,
        PERIOD,
        SLASH,
        
        ZERO,
        ONE,
        TWO,
        THREE,
        FOUR,
        FIVE,
        SIX,
        SEVEN,
        EIGHT,
        NINE,

        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,

        LEFT_BRACKET,
        RIGHT_BRACKET,
        BACKSLASH,
        BACKTICK,
        SEMICOLON,
        EQUAL,

        ESCAPE,
        ENTER,
        TAB,
        BACKSPACE,
        INSERT,
        DEL,
        RIGHT,
        LEFT,
        DOWN,
        UP,
        PAGE_UP,
        PAGE_DOWN,
        HOME,
        END,
        PAUSE,
        
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        F13,

        NUM_ZERO,
        NUM_ONE,
        NUM_TWO,
        NUM_THREE,
        NUM_FOUR,
        NUM_FIVE,
        NUM_SIX,
        NUM_SEVEN,
        NUM_EIGHT,
        NUM_NINE,
        NUM_DECIMAL,
        NUM_DIVIDE,
        NUM_MULTIPLY,
        NUM_SUBTRACT,
        NUM_ADD,
        NUM_ENTER,
        NUM_EQUAL,

        LEFT_SHIFT,
        RIGHT_SHIFT,
        LEFT_CONTROL,
        RIGHT_CONTROL,
        LEFT_ALT,
        RIGHT_ALT,
        LEFT_SUPER,
        RIGHT_SUPER,

        COUNT
    };

    using KeyboardModifiers_t = uint8_t;
    namespace KeyboardModifiers {
        constexpr KeyboardModifiers_t CONTROL = 1;
        constexpr KeyboardModifiers_t SHIFT = 1 << 1;
        constexpr KeyboardModifiers_t ALT = 1 << 2;
        constexpr KeyboardModifiers_t SUPER = 1 << 3;
    }
}

#endif
