/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/util/Time.hpp>

#include <chrono>

namespace ff {
    timestamp_t getCurrentTimestamp() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
    }

    timestamp_t roundTimestampToSeconds(const timestamp_t& ts) {
        return (timestamp_t)glm::round(ts / 1000.0) * 1000;
    }
}
