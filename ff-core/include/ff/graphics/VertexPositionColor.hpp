/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_VERTEX_POSITION_COLOR_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_VERTEX_POSITION_COLOR_HPP

#include <glm/glm.hpp>
#include <ff/graphics/Color.hpp>

namespace ff {
    struct VertexPositionColor {
        glm::vec4 position;
        ff::Color color;
    };
}

#endif
