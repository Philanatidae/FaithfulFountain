/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/PPQuad.hpp>

#include <ff/Console.hpp>
#include <ff/Locator.hpp>

#include <array>

#include <ff/graphics/VertexPositionTextureColor.hpp>

namespace ff {
    PPQuad::PPQuad(const std::shared_ptr<IShader>& shader,
        const ColorBufferFormat& renderTargetColorBufferFormat) {
        std::array<VertexPositionTextureColor, 4> vertices({
           VertexPositionTextureColor{
               glm::vec4(-1, -1, 0, 1),
               ff::Locator::getGraphicsDevice().processUVCoords(0, 1),
               ff::Color(1, 1, 1 ,1)
           },
           VertexPositionTextureColor{
                glm::vec4(1, -1, 0, 1),
                ff::Locator::getGraphicsDevice().processUVCoords(1, 1),
                ff::Color(1, 1, 1 ,1)
           },
           VertexPositionTextureColor{
                glm::vec4(1, 1, 0, 1),
                ff::Locator::getGraphicsDevice().processUVCoords(1, 0),
                ff::Color(1, 1, 1 ,1)
           },
           VertexPositionTextureColor{
                glm::vec4(-1, 1, 0, 1),
                ff::Locator::getGraphicsDevice().processUVCoords(0, 0),
                ff::Color(1, 1, 1 ,1)
           }
        });
        
        std::array<typename IndexBuffer<>::type, 6> indices({
           0, 1, 2, 0, 2, 3
        });

        _verts = ff::Locator::getGraphicsDevice().createVertexBuffer<VertexPositionTextureColor>(vertices.size());
        _verts->bufferData(vertices.data(), vertices.size());

        _indices = ff::Locator::getGraphicsDevice().createIndexBuffer(indices.size());
        _indices->bufferData(&indices[0], indices.size());
        _indicesCount = indices.size();

        // Build vertex descriptor
        VertexDescriptorInfo vertexDescriptorInfo({
            BufferDescriptorInfo(0, sizeof(VertexPositionTextureColor), {
                AttributeDescriptorInfo(0, 0, AttributeFormat::Float4), // Position
                AttributeDescriptorInfo(1, sizeof(glm::vec4), AttributeFormat::Float2), // TexCoord
                AttributeDescriptorInfo(2, sizeof(glm::vec4) + sizeof(glm::vec2), AttributeFormat::Float4), // Color
            })
        });
        auto vertexDescriptor = ff::Locator::getGraphicsDevice().createVertexDescriptor(vertexDescriptorInfo);

        _renderState = ff::Locator::getGraphicsDevice().createRenderState(shader,
            vertexDescriptor,
            {
                RenderColorLayerState(renderTargetColorBufferFormat)
            });
    }
    PPQuad::~PPQuad() {
    }

    void PPQuad::setRenderState(const std::shared_ptr<IRenderState>& renderState) {
        _renderState = renderState;
    }

    void PPQuad::draw(IRenderPass& renderPass) {
        renderPass.setState(_renderState);
        renderPass.setVertexBuffer(_verts, 0);

        renderPass.drawIndexedPrimitives(RenderPrimitives::TRIANGLE_LIST,
                                         _indices,
                                         _indicesCount);
    }

    void PPQuad::fillCamera(const OrthographicCamera& camera) {
        std::array<VertexPositionTextureColor, 4> vertices({
           VertexPositionTextureColor{
               glm::vec4(-1, -1, 0, 1),
               ff::Locator::getGraphicsDevice().processUVCoords(glm::vec2(0, 1)),
               ff::Color(1, 1, 1 ,1)
           },
           VertexPositionTextureColor{
                glm::vec4(1, -1, 0, 1),
                ff::Locator::getGraphicsDevice().processUVCoords(glm::vec2(1, 1)),
                ff::Color(1, 1, 1 ,1)
           },
           VertexPositionTextureColor{
                glm::vec4(1, 1, 0, 1),
                ff::Locator::getGraphicsDevice().processUVCoords(glm::vec2(1, 0)),
                ff::Color(1, 1, 1 ,1)
           },
           VertexPositionTextureColor{
                glm::vec4(-1, 1, 0, 1),
                ff::Locator::getGraphicsDevice().processUVCoords(glm::vec2(0, 0)),
                ff::Color(1, 1, 1 ,1)
           }
        });

        vertices[0].position = camera.getTransformMatrix() * camera.getCombinedInverse() * vertices[0].position;
        vertices[1].position = camera.getTransformMatrix() * camera.getCombinedInverse() * vertices[1].position;
        vertices[2].position = camera.getTransformMatrix() * camera.getCombinedInverse() * vertices[2].position;
        vertices[3].position = camera.getTransformMatrix() * camera.getCombinedInverse() * vertices[3].position;

        _verts->bufferData(&vertices[0], vertices.size());
    }
}
