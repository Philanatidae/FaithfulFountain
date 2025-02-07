/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/IdentityTextureSource.hpp>

namespace ff {
    std::array<uint8_t, 4> getIdentityTextureSource() {
        return std::array<uint8_t, 4>({
            0xFF,0xFF,0xFF,0xFF
        });
    }
}
