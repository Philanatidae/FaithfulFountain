/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLVertexDescriptor.hpp>
#include <stdint.h>

#include <ff-graphics-opengl/GLVertexBufferImp.hpp>

#include <ff/Console.hpp>

namespace ff {
    void convertAttributeFormatToGLFormat(const AttributeFormat& format, int& o_size, GLenum& o_type) {
        switch(format) {
        case AttributeFormat::Float:
            o_type = GL_FLOAT;
            o_size = 1;
            return;
        case AttributeFormat::Float2:
            o_type = GL_FLOAT;
            o_size = 2;
            return;
        case AttributeFormat::Float3:
            o_type = GL_FLOAT;
            o_size = 3;
            return;
        case AttributeFormat::Float4:
            o_type = GL_FLOAT;
            o_size = 4;
            return;

        case AttributeFormat::Int:
            o_type = GL_INT;
            o_size = 1;
            return;
        case AttributeFormat::Int2:
            o_type = GL_INT;
            o_size = 2;
            return;
        case AttributeFormat::Int3:
            o_type = GL_INT;
            o_size = 3;
            return;
        case AttributeFormat::Int4:
            o_type = GL_INT;
            o_size = 4;
            return;

        case AttributeFormat::UInt:
            o_type = GL_UNSIGNED_INT;
            o_size = 1;
            return;
        case AttributeFormat::UInt2:
            o_type = GL_UNSIGNED_INT;
            o_size = 2;
            return;
        case AttributeFormat::UInt3:
            o_type = GL_UNSIGNED_INT;
            o_size = 3;
            return;
        case AttributeFormat::UInt4:
            o_type = GL_UNSIGNED_INT;
            o_size = 4;
            return;

        case AttributeFormat::Byte:
            o_type = GL_BYTE;
            o_size = 1;
            return;
        case AttributeFormat::Byte2:
            o_type = GL_BYTE;
            o_size = 2;
            return;
        case AttributeFormat::Byte3:
            o_type = GL_BYTE;
            o_size = 3;
            return;
        case AttributeFormat::Byte4:
            o_type = GL_BYTE;
            o_size = 4;
            return;

        case AttributeFormat::UByte:
            o_type = GL_UNSIGNED_BYTE;
            o_size = 1;
            return;
        case AttributeFormat::UByte2:
            o_type = GL_UNSIGNED_BYTE;
            o_size = 2;
            return;
        case AttributeFormat::UByte3:
            o_type = GL_UNSIGNED_BYTE;
            o_size = 3;
            return;
        case AttributeFormat::UByte4:
            o_type = GL_UNSIGNED_BYTE;
            o_size = 4;
            return;

        default:
            FF_CONSOLE_ERROR("Unable to convert attribute format to OpenGL format.");
        }
    }

    GLVertexDescriptor::GLVertexDescriptor(GLGraphicsDevice* const& graphicsDevice,
        const VertexDescriptorInfo& info)
        :_graphicsDevice(graphicsDevice),
        IVertexDescriptor(info) {
        for(BufferDescriptorInfo const& bufferDescriptorInfo : info.buffers) {
            _slotInfo.emplace(bufferDescriptorInfo.slot, bufferDescriptorInfo);
            _slots.insert(bufferDescriptorInfo.slot);
        }
    }

    void GLVertexDescriptor::createVertexAttributePointers(std::unordered_map<int, std::shared_ptr<IVertexBuffer>>& bufferSlots,
        std::unordered_map<int, int>& bufferOffsets) {
        for(BufferDescriptorInfo const& bufferDescriptorInfo : getDescriptorInfo().buffers) {
            std::shared_ptr<IVertexBuffer> buffer;
            auto bufferSlotIt = bufferSlots.find(bufferDescriptorInfo.slot);
            if(bufferSlotIt != bufferSlots.end()) {
                buffer = bufferSlotIt->second;
            }
            if(!buffer) {
                continue;
            }

            auto bufferOffsetIt = bufferOffsets.find(bufferDescriptorInfo.slot);
            int offset = 0;
            if(bufferOffsetIt != bufferOffsets.end()) {
                offset = bufferOffsetIt->second;
            }

            createVertexAttributePointerForSlot(bufferDescriptorInfo.slot, buffer, offset);
        }
    }
    void GLVertexDescriptor::createVertexAttributePointerForSlot(const int& slot,
        const std::shared_ptr<IVertexBuffer>& buffer,
        int const& offset) {
        auto bufferInfoIt = _slotInfo.find(slot);
        if(bufferInfoIt == _slotInfo.end()) {
            return;
        }
        BufferDescriptorInfo& bufferDescriptorInfo = bufferInfoIt->second;

        GLVertexBufferImp& bufferImp = static_cast<GLVertexBufferImp&>(*buffer->getImp());
        bufferImp.bind();

        for(AttributeDescriptorInfo attribDescriptorInfo : bufferDescriptorInfo.attributes) {
            int size; // out
            GLenum type; // out
            convertAttributeFormatToGLFormat(attribDescriptorInfo.format, size, type);

            switch(type) {
                case GL_FLOAT:
                case GL_HALF_FLOAT:
                    FF_GL_CALL(glVertexAttribPointer, attribDescriptorInfo.location,
                        size,
                        type,
                        GL_FALSE,
                        bufferDescriptorInfo.stride,
                        (void*)((intptr_t)(attribDescriptorInfo.offset + offset)));
                    break;
                case GL_BYTE:
                case GL_UNSIGNED_BYTE:
                case GL_SHORT:
                case GL_UNSIGNED_SHORT:
                case GL_INT:
                case GL_UNSIGNED_INT:
                    FF_GL_CALL(glVertexAttribIPointer, attribDescriptorInfo.location,
                        size,
                        type,
                        bufferDescriptorInfo.stride,
                        (void*)((intptr_t)(attribDescriptorInfo.offset + offset)));
                    break;
                default:
                    FF_CONSOLE_FATAL("Type not supported: `%s`.", (int)type);
                    break;
            }
            FF_GL_CALL(glEnableVertexAttribArray, attribDescriptorInfo.location);
        }
    }

    const std::unordered_map<int, BufferDescriptorInfo>& GLVertexDescriptor::getSlotInfo() {
        return _slotInfo;
    }
    const std::set<int>& GLVertexDescriptor::getSlots() {
        return _slots;
    }
}
