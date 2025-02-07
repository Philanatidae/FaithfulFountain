/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_FSQUAD_HPP
#define _FAITHFUL_FOUNTAIN_FSQUAD_HPP

#include <ff/graphics/BufferFormats.hpp>
#include <ff/graphics/IShader.hpp>
#include <ff/graphics/VertexBuffer.hpp>
#include <ff/graphics/IndexBuffer.hpp>
#include <ff/graphics/VertexBuffer.hpp>
#include <ff/graphics/IRenderState.hpp>

#include <ff/graphics/IRenderPass.hpp>

#include <ff/graphics/Color.hpp>

#include <glm/glm.hpp>

#include <ff/graphics/OrthographicCamera.hpp>

#include <ff/graphics/VertexPositionTextureColor.hpp>

namespace ff {
    class PPQuad {
    public:
        PPQuad(const std::shared_ptr<IShader>& shader,
            const ColorBufferFormat& renderTargetColorBufferFormat);
        virtual ~PPQuad();

        void setRenderState(const std::shared_ptr<IRenderState>& renderState);

        void draw(IRenderPass& renderPass);
        
        void fillCamera(const OrthographicCamera& camera);

    private:
        std::shared_ptr<VertexBuffer<VertexPositionTextureColor>> _verts;

        std::shared_ptr<IRenderState> _renderState;

        std::shared_ptr<IndexBuffer<>> _indices;
        int _indicesCount;
    };
}

#endif
