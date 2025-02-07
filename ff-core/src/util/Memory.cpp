/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/util/Memory.hpp>

namespace ff {
    namespace _internal {
        std::unordered_map<void**, CountDeleter>& getRefCountMap() {
            static std::unordered_map<void**, CountDeleter> map;
            return map;
        }
    }
}
