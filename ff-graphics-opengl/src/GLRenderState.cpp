/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLRenderState.hpp>

#include <ff-graphics-opengl/GLBlendStateHelpers.hpp>
#include <ff-graphics-opengl/GLVertexBufferImp.hpp>
#include <ff-graphics-opengl/GLIndexBufferImp.hpp>
#include <ff-graphics-opengl/GLGlobalState.hpp>
#include <ff-graphics-opengl/GLFramebufferManager.hpp>

#include <ff/util/Hash.hpp>
#include <ff/Console.hpp>

namespace ff {
    GLRenderState::GLRenderState(GLGraphicsDevice* const& graphicsDevice,
        std::shared_ptr<IShader> const& shader,
        std::shared_ptr<IVertexDescriptor> const& vertexDescriptor,
        std::vector<RenderColorLayerState> const& renderColorLayerStates,
        std::optional<DepthBufferFormat> const& depthBufferFormat)
        :IRenderState(shader,
            vertexDescriptor,
            renderColorLayerStates,
            depthBufferFormat),
        _graphicsDevice(graphicsDevice) {
    }

    GLRenderState::~GLRenderState() {
        for(auto& pair : _vaos) {
            FF_GL_CALL(glDeleteVertexArrays, 1, &pair.second);
        }
    }

    void GLRenderState::prepareForDrawCall(std::unordered_map<int, std::shared_ptr<IVertexBuffer>>& bufferSlots,
        std::unordered_map<int, int>& bufferOffsets,
        IIndexBuffer* const& indexBuffer) {
        for(int i = 0; i < getRenderColorLayerStates().size(); i++) {
            _graphicsDevice->getFramebufferManager()->setDrawFBOBlendState(i,getRenderColorLayerStates()[i].blendState);
        }

        GLShader& glShader = *static_cast<GLShader*>(getShader().get());

        // Create a unique identifier for this particular set of slot-buffer pairs
        uint64_t n = 0;
        GLVertexDescriptor& glVertexDescriptor = *static_cast<GLVertexDescriptor*>(getVertexDescriptor().get());
        for(auto it = glVertexDescriptor.getSlots().begin();
            it != glVertexDescriptor.getSlots().end();
            it++) {
            FF_ASSERT(bufferSlots.find(*it) != bufferSlots.end(), "Missing vertex buffer for slot %s.", *it);
            hash_combine_num(n, *it);
            hash_combine_num(n, static_cast<GLVertexBufferImp*>(bufferSlots[*it]->getImp())->getGLBuffer());
            if (bufferOffsets.find(*it) != bufferOffsets.end()) {
                hash_combine_num(n, bufferOffsets[*it]);
            }
        }

        // Both paths in this if statement will bind the VAO
        if(_vaos.find(n) == _vaos.end()) {
            GLuint vao;
            FF_GL_CALL(glGenVertexArrays, 1, &vao);
            _graphicsDevice->getGlobalState()->bindVAO(vao);
            glVertexDescriptor.createVertexAttributePointers(bufferSlots, bufferOffsets);

            _vaos.emplace(n, vao);
        } else {
            _graphicsDevice->getGlobalState()->bindVAO(_vaos[n]);
        }

        // For drawing, we'll bind index buffers here. This ensures
        // that the index buffer is bound after the vao, without
        // making things too complicated in GLGlobalState (also
        // makes sure that we don't bind an index buffer when
        // simply switching VAOs if we don't need to bind it).
        if(indexBuffer != nullptr) {
            GLIndexBufferImp* indexBufferImp = static_cast<GLIndexBufferImp*>(indexBuffer->getImp());
            _graphicsDevice->getGlobalState()->bindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                indexBufferImp->getGLBuffer());
        }
        
        // @todo Cleanup VAOs that are no longer used (possibly with weak_ptr and an event on GLVertexBuffer destruction?)
    }
}
