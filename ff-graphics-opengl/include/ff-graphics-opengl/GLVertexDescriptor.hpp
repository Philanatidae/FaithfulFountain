/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_VERTEX_DESCRIPTOR_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_VERTEX_DESCRIPTOR_HPP

#include <ff-graphics-opengl/gl.hpp>
#include <ff-graphics-opengl/GLGraphicsDevice.hpp>

#include <ff/graphics/IVertexDescriptor.hpp>
#include <ff/graphics/VertexBuffer.hpp>

#include <unordered_map>
#include <memory>

namespace ff {
    void convertAttributeFormatToGLFormat(const AttributeFormat& format, int& o_size, GLenum& o_type);

    class GLVertexDescriptor final : public IVertexDescriptor {
    public:
        GLVertexDescriptor(GLGraphicsDevice* const& graphicsDevice,
            const VertexDescriptorInfo& info);
        ~GLVertexDescriptor() = default;

        void createVertexAttributePointers(std::unordered_map<int, std::shared_ptr<IVertexBuffer>>& bufferSlots,
            std::unordered_map<int, int>& bufferOffsets);
        void createVertexAttributePointerForSlot(const int& slot,
            const std::shared_ptr<IVertexBuffer>& buffer, 
            int const& offset);

        const std::unordered_map<int, BufferDescriptorInfo>& getSlotInfo();
        const std::set<int>& getSlots();

    private:
        GLGraphicsDevice* const _graphicsDevice;

        std::unordered_map<int, BufferDescriptorInfo> _slotInfo;
        std::set<int> _slots;
    };
}

#endif
