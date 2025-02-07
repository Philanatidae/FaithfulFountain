/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_MESH_TREE_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_MESH_TREE_HPP

#include <ff/graphics/Mesh.hpp>
#include <ff/graphics/Material.hpp>
#include <ff/resources/ResourceHandle.hpp>

#include <vector>

namespace ff {

struct MeshTreeNode {
    ResourceHandle<Mesh> mesh;
    ResourceHandle<Material> material;

    std::vector<std::unique_ptr<MeshTreeNode>> children;
};

using MeshTree = MeshTreeNode;

}

#endif
