/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_COMPONENTS_MODEL_COMPONENT_HPP
#define _FAITHFUL_FOUNTAIN_COMPONENTS_MODEL_COMPONENT_HPP

#include <ff/actors/Component.hpp>
#include <ff/graphics/Mesh.hpp>
#include <ff/graphics/Material.hpp>
#include <ff/resources/ResourceHandle.hpp>
#include <ff/graphics/VertexBuffer.hpp>
#include <ff/graphics/MeshUniforms.hpp>
#include <ff/graphics/RenderGroup.hpp>
#include <memory>
#include <stdint.h>

namespace ff {

struct ActorUniforms {
    uint64_t actorID;
};

struct ModelComponent : public Component<ModelComponent> {
    ModelComponent(ResourceHandle<Mesh> const& mesh = ResourceHandle<Mesh>::createNullResource(),
        ResourceHandle<Material> const& material = ResourceHandle<Material>::createNullResource())
        :mesh(mesh),
        material(material),
        hidden(false),
        renderGroup(ff::RENDER_GROUP_1) {
    }

    ResourceHandle<Mesh> mesh;
    ResourceHandle<Material> material;
    std::shared_ptr<ff::UniformBuffer<ff::MeshUniforms>> meshUniforms;
    std::shared_ptr<ff::UniformBuffer<ActorUniforms>> actorUniforms;
    ActorUniforms actorUniforms_val;
    bool hidden;
    ff::RenderGroup renderGroup;
};

}

#endif
