/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_UTIL_TIME_HPP
#define _FAITHFUL_FOUNTAIN_UTIL_TIME_HPP

#include <stdint.h>
#include <glm/glm.hpp>

namespace ff {
    using timestamp_t = uint64_t;

    timestamp_t getCurrentTimestamp();
    timestamp_t roundTimestampToSeconds(const timestamp_t& ts);
}

#endif
