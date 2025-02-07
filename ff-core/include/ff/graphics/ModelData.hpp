/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_MODEL_DATA_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_MODEL_DATA_HPP

#include <ff/graphics/MeshTree.hpp>

#include <ff/assets/IAssetBundle.hpp>
#include <ff/io/BinaryMemory.hpp>
#include <ff/actors/Actor.hpp>

#include <cgltf.h>

namespace ff {

class ModelData final {
public:
    ModelData(IAssetBundle& assetBundle, const nlohmann::json& assetObject);

    std::unique_ptr<MeshTree> meshTree;

    Actor_t createModelActor();

private:
    void processNode(cgltf_node* const& gnode,
        MeshTreeNode* const& parentMeshNode,
        IAssetBundle& assetBundle,
        nlohmann::json const& assetObject,
        BinaryMemory* const& modelMem);

    // void verifyBufferData(cgltf_buffer_view )
};

}

#endif
