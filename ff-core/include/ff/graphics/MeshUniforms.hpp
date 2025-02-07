/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_MESH_UNIFORMS_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_MESH_UNIFORMS_HPP

#include <glm/glm.hpp>

namespace ff {

struct MeshUniforms {
    MeshUniforms(glm::mat4 const& worldMat)
        :worldMat(worldMat) {
    }

    glm::mat4 worldMat;
};

}

#endif
