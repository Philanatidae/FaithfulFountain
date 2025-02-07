/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_MESSAGES_MESSAGE_LISTENER_PRIORITY_HPP
#define _FAITHFUL_FOUNTAIN_MESSAGES_MESSAGE_LISTENER_PRIORITY_HPP

#include <stdint.h>

namespace ff {

using MessageListenerPriority_t = uint64_t;

namespace MessageListenerPriority {
    constexpr MessageListenerPriority_t EXTREMELY_LOW = (MessageListenerPriority_t)1;
    constexpr MessageListenerPriority_t VERY_LOW = (MessageListenerPriority_t)100;
    constexpr MessageListenerPriority_t LOW = (MessageListenerPriority_t)10000;
    constexpr MessageListenerPriority_t MEDIUM_LOW = (MessageListenerPriority_t)1000000;
    constexpr MessageListenerPriority_t MEDIUM = (MessageListenerPriority_t)100000000;
    constexpr MessageListenerPriority_t MEDIUM_HIGH = (MessageListenerPriority_t)10000000000;
    constexpr MessageListenerPriority_t HIGH = (MessageListenerPriority_t)1000000000000;
    constexpr MessageListenerPriority_t VERY_HIGH = (MessageListenerPriority_t)100000000000000;
    constexpr MessageListenerPriority_t EXTREMELY_HIGH = (MessageListenerPriority_t)10000000000000000;

    constexpr MessageListenerPriority_t DEFAULT = MEDIUM;
}

}

#endif
