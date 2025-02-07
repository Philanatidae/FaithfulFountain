/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_VERTEX_DESCRIPTOR_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_VERTEX_DESCRIPTOR_HPP

#include <vector>

namespace ff {
    enum class AttributeFormat {
        Float,
        Float2,
        Float3,
        Float4,

        Int,
        Int2,
        Int3,
        Int4,

        UInt,
        UInt2,
        UInt3,
        UInt4,

        Byte,
        Byte2,
        Byte3,
        Byte4,

        UByte,
        UByte2,
        UByte3,
        UByte4
    };

    struct AttributeDescriptorInfo {
        int location;
        int offset;
        AttributeFormat format;

        AttributeDescriptorInfo(const int& location, const int& offset, const AttributeFormat& format)
            :location(location),offset(offset),format(format) {
        }
        AttributeDescriptorInfo(const AttributeDescriptorInfo& other)
            :location(other.location),offset(other.offset),format(other.format) {
        }
    };
    struct BufferDescriptorInfo {
        int slot;
        int stride;
        std::vector<AttributeDescriptorInfo> attributes;

        BufferDescriptorInfo(const int& slot, const int& stride, const std::vector<AttributeDescriptorInfo>& attributes)
            :slot(slot),stride(stride),attributes(attributes) {
        }
        BufferDescriptorInfo(const BufferDescriptorInfo& other)
            :slot(other.slot),stride(other.stride),attributes(other.attributes) {
        }
    };
    struct VertexDescriptorInfo {
        std::vector<BufferDescriptorInfo> buffers;

        VertexDescriptorInfo(const std::vector<BufferDescriptorInfo>& buffers)
            :buffers(buffers) {
        }
        VertexDescriptorInfo(const VertexDescriptorInfo& other)
            :buffers(other.buffers) {
        }
    };

    // @todo Have the abstraction work with ff-asset-builder to verify
    // that an attribute type can be used with the shader attribute location
    // (Metal complains if they aren't castable, OpenGL will happily cast
    // all day long without error). Needs to be an assert on the engine level
    // to cover all abstractions.
    class IVertexDescriptor {
    public:
        IVertexDescriptor(VertexDescriptorInfo const& info);
        virtual ~IVertexDescriptor() = default;

        VertexDescriptorInfo getDescriptorInfo() const;

    private:
        VertexDescriptorInfo _info;
    };

    class NullVertexDescriptor : public IVertexDescriptor {
    public:
        NullVertexDescriptor(const VertexDescriptorInfo& descriptorInfo):IVertexDescriptor(descriptorInfo) { }
        virtual ~NullVertexDescriptor() = default;
    };
}

#endif
