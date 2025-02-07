/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_MESH_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_MESH_HPP

#include <ff/graphics/VertexBuffer.hpp>
#include <ff/graphics/IndexBuffer.hpp>

#include <ff/graphics/VertexPositionTextureColor.hpp>

namespace ff {

struct Mesh {
    Mesh(std::shared_ptr<ff::VertexBuffer<ff::VertexPositionTextureColor>> const& vertices,
        std::shared_ptr<ff::IndexBuffer<>> const& indices)
        :vertices(vertices),
        indices(indices) {
    }

    std::shared_ptr<ff::VertexBuffer<ff::VertexPositionTextureColor>> vertices;
    std::shared_ptr<ff::IndexBuffer<>> indices;
};

}

#endif
