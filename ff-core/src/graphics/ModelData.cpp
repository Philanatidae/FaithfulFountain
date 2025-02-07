/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/ModelData.hpp>

#include <cgltf.h>

#include <cstdlib>

#include <ff/io/BinaryMemory.hpp>

#include <ff/graphics/Mesh.hpp>
#include <ff/graphics/VertexPositionTextureColor.hpp>

#include <ff/Locator.hpp>

#include <ff/components/TransformComponent.hpp>
#include <ff/components/ModelComponent.hpp>

namespace ff {

ModelData::ModelData(IAssetBundle& assetBundle, const nlohmann::json& assetObject) {
    FF_ASSET_TYPE_CHECK(assetObject, "Model")

    FF_ASSERT(assetObject.contains("path"), "Missing `path` in asset object.");
    auto modelMem = assetBundle.load<BinaryMemory>(assetObject["path"]);

    cgltf_options options = {};
    std::memset(&options, 0, sizeof(cgltf_options));

    options.type = cgltf_file_type_glb;

    cgltf_data* data = nullptr;
    FF_ASSERT(cgltf_parse(&options, modelMem->data(), modelMem->size(), &data) == cgltf_result_success,
        "GLTF (glb) file could not be opened for Model %s.", assetObject["name"]);

    // cgltf doesn't load buffers automatically, we have to do this manually
    // ff-asset-builder guarantees that buffers_count == 1
    FF_ASSERT(data->buffers_count == 1, "Buffer count must be 1.");
    FF_ASSERT(data->buffers[0].data == nullptr, "Buffer already has data."); // Sanity check
    FF_ASSERT(data->bin_size >= data->buffers[0].size, "GLTF buffer size is greater than the data available in memory.");
    data->buffers[0].data = (void*)data->bin;
    data->buffers[0].data_free_method = cgltf_data_free_method_none;

    // Find root node.
    // For now, we are guaranteed only one root node.
    // This may change in the future. Currently this is
    // being enforced by ff-asset-builder.
    cgltf_node* rootNode = nullptr;
    for(int i = 0; i < data->nodes_count; i++) {
        if(data->nodes[i].parent == nullptr) {
            rootNode = &data->nodes[i];
            break;
        }
    }

    // Turn into meshes
    processNode(rootNode, nullptr, assetBundle, assetObject, modelMem.get());
}

Actor_t ModelData::createModelActor() {
    // Only root for now
    // @todo Parenting

    Actor_t actor = Locator::getActorManager().createActor();

    Locator::getActorManager().addComponent<TransformComponent>(actor);
    Locator::getActorManager().addComponent<ModelComponent>(actor, meshTree->mesh, meshTree->material);

    return actor;
}

void ModelData::processNode(cgltf_node* const& gnode,
    MeshTreeNode* const& parentMeshNode,
    IAssetBundle& assetBundle,
    nlohmann::json const& assetObject,
    BinaryMemory* const& modelMem) {
    if(gnode->mesh == nullptr) {
        return;
    }

    std::unique_ptr<MeshTreeNode> meshNode = std::make_unique<MeshTreeNode>();

    std::vector<uint32_t> indices; // @todo Automatically decide to use 16 or 32-bit indices
    std::vector<VertexPositionTextureColor> vertices;

    cgltf_mesh const& mesh = *gnode->mesh;
    FF_ASSERT(mesh.primitives_count == 1, "One primitive list per mesh currently allowed.");

    cgltf_primitive const& primitive = mesh.primitives[0];

    cgltf_attribute* posAttrib = nullptr; // Required, either 3 or 4 component
    cgltf_attribute* normAttrib = nullptr; // Optional, calculated if D.N.E., at least 3-component
    cgltf_attribute* texAttrib = nullptr; // Optional, 2-component
    cgltf_attribute* colorAttrib = nullptr; // Optional, 3 or 4 component

    for(int i = 0; i < primitive.attributes_count; i++) {
        switch(primitive.attributes[i].type) {
            case cgltf_attribute_type_position:
                posAttrib = &primitive.attributes[i];
                break;
            case cgltf_attribute_type_normal:
                normAttrib = &primitive.attributes[i];
                break;
            case cgltf_attribute_type_texcoord:
                texAttrib = &primitive.attributes[i];
                break;
            case cgltf_attribute_type_color:
                colorAttrib = &primitive.attributes[i];
                break;
            default:
                continue;
        }
    }

    FF_ASSERT(posAttrib != nullptr, "Position attribute is required.");
    FF_ASSERT(posAttrib->data->type == cgltf_type_vec3 || posAttrib->data->type == cgltf_type_vec4,
        "Position attribute must have 3 or 4 components.");

    if(texAttrib == nullptr && colorAttrib == nullptr) {
        FF_CONSOLE_WARN("Model does not contain per-vertex color information. White vertex painting will be used.");
    }

    if(texAttrib != nullptr) {
        FF_ASSERT(texAttrib->data->type == cgltf_type_vec2,
            "Texture coordination attribute must have 2 components.");
    }
    if(colorAttrib != nullptr) {
        FF_ASSERT(colorAttrib->data->type == cgltf_type_vec3 || colorAttrib->data->type == cgltf_type_vec4,
            "Color attribute must have 3 or 4 components.");
    }

    if(normAttrib != nullptr) {
        FF_ASSERT(normAttrib->data->type == cgltf_type_vec3,
            "Normal attribute must have 3 components.");
    }
    // @todo If normal do not exist, calculate

    for(int i = 0; i < primitive.indices->count; i++) {
        cgltf_uint index;
        FF_ASSERT(cgltf_accessor_read_uint(primitive.indices, i, &index, 1), "Could not read uint from primitive indices.");
        indices.push_back((uint32_t)index);
    }

    for(int i = 0; i < posAttrib->data->count; i++) {
        glm::vec4 pos(-1, -1, -1, 1);
        FF_ASSERT(cgltf_accessor_read_float(posAttrib->data, i, &pos.x, cgltf_type_vec3 ? 3 : 4), "Could not read float[3/4] from position attribute.");

        glm::vec2 texCoord(0, 0);
        if(texAttrib != nullptr) {
            FF_ASSERT(cgltf_accessor_read_float(texAttrib->data, i, &texCoord.x, 2), "Could not read float[2] from texture coordinate attribute.");
        }

        Color color = Color::white();
        if(colorAttrib != nullptr) {
            FF_ASSERT(cgltf_accessor_read_float(colorAttrib->data, i, &color.r, cgltf_type_vec3 ? 3 : 4), "Could not read float[3/4] from texture coordinate attribute.");
        }

        // @todo Normals
        
        vertices.push_back(VertexPositionTextureColor(pos,
            Locator::getGraphicsDevice().processUVCoords(texCoord),
            color
        ));
    }

    auto vertexBuffer = Locator::getGraphicsDevice().createVertexBuffer<VertexPositionTextureColor>(vertices.size());
    vertexBuffer->bufferData(vertices.data(), vertices.size());

    auto indexBuffer = Locator::getGraphicsDevice().createIndexBuffer(indices.size());
    indexBuffer->bufferData(indices.data(), indices.size());

    meshNode->mesh = ResourceHandle<Mesh>::createResource([vertexBuffer, indexBuffer]() -> Mesh* {
        return new Mesh(vertexBuffer, indexBuffer);
    });
    
    if(assetObject["material-assignments"].find(mesh.name) == assetObject["material-assignments"].end()) {
        if(texAttrib == nullptr) {
            meshNode->material = Locator::getGraphicsDevice().getIdentityMaterial();
        } else {
            meshNode->material = Locator::getGraphicsDevice().getNullMaterial();
        }
    } else {
        meshNode->material = assetBundle.load<Material>(assetObject["material-assignments"][mesh.name]);
    }

    for(int i = 0; i < gnode->children_count; i++) {
        processNode(gnode->children[i], meshNode.get(), assetBundle, assetObject, modelMem);
    }

    if(parentMeshNode == nullptr) {
        meshTree = std::move(meshNode);
    } else {
        parentMeshNode->children.emplace_back(std::move(meshNode));
    }
}

}
