/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_EVENTS_ENV_ENV_TEXT_INPUT_CHARACTER_EVENT_HPP
#define _FAITHFUL_FOUNTAIN_EVENTS_ENV_ENV_TEXT_INPUT_CHARACTER_EVENT_HPP

#include <ff/messages/Event.hpp>
#include <tinyformat/tinyformat.h>

namespace ff {
    FF_EVENT_DEFINE_1(EnvTextInputCharacterEvent,
        "evt_env_text_input_character",
        char, c,
        "%s",
        c);
}

#endif
