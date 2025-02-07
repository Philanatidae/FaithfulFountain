/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_VERTEX_POSITION_TEXTURE_COLOR_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_VERTEX_POSITION_TEXTURE_COLOR_HPP

#include <glm/glm.hpp>
#include <ff/graphics/Color.hpp>

namespace ff {
    struct VertexPositionTextureColor {
        VertexPositionTextureColor(glm::vec4 const& position,
            glm::vec2 const& texCoord,
            ff::Color const& color)
            :position(position),
            texCoord(texCoord),
            color(color) {
        }
        glm::vec4 position;
        glm::vec2 texCoord;
        ff::Color color;
    };
}

#endif
