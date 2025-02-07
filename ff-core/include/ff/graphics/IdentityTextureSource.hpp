/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_IDENTITY_TEXTURE_SOURCE_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_IDENTITY_TEXTURE_SOURCE_HPP

#include <stdint.h>
#include <array>

namespace ff {
    std::array<uint8_t, 4> getIdentityTextureSource();
}

#endif
