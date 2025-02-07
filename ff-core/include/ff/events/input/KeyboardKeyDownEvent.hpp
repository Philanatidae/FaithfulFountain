/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_EVENTS_INPUT_KEYBOARD_KEY_DOWN_EVENT_HPP
#define _FAITHFUL_FOUNTAIN_EVENTS_INPUT_KEYBOARD_KEY_DOWN_EVENT_HPP

#include <ff/input/Keyboard.hpp>
#include <ff/messages/Event.hpp>
#include <tinyformat/tinyformat.h>

#include <bitset>

namespace ff {

    FF_EVENT_DEFINE_2(KeyboardKeyDownEvent,
        "evt_keyboard_key_down",
        KeyboardKey, key,
        KeyboardModifiers_t, mods,
        "%s + %s",
        (int)key,
        std::bitset<8*sizeof(KeyboardModifiers_t)>(mods));

}

#endif
