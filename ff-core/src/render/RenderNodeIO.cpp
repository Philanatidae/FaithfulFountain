/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/render/RenderNodeIO.hpp>

namespace ff {

char const* convertRenderNodeIOTypeToFriendlyString(RenderNodeIOType const& type) {
    switch(type) {
    case RenderNodeIOType::INT:
        return "Int";
    case RenderNodeIOType::FLOAT:
        return "Float";
    case RenderNodeIOType::VEC2:
        return "Vec2";
    case RenderNodeIOType::VEC3:
        return "Vec3";
    case RenderNodeIOType::VEC4:
        return "Vec4";
    case RenderNodeIOType::MATRIX:
        return "Matrix";
    case RenderNodeIOType::COLOR:
        return "Color";
    default:
        return "Unknown";
    }
}

}
