/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-metal/MetalVertexDescriptor.hmm>

#include <ff-graphics-metal/MetalConstants.hmm>

namespace ff {
    MTLVertexFormat convertAttributeFormatToMetalFormat(const AttributeFormat& format) {
        switch(format) {
        case AttributeFormat::Float:
            return MTLVertexFormatFloat;
        case AttributeFormat::Float2:
            return MTLVertexFormatFloat2;
        case AttributeFormat::Float3:
            return MTLVertexFormatFloat3;
        case AttributeFormat::Float4:
            return MTLVertexFormatFloat4;

        case AttributeFormat::Int:
            return MTLVertexFormatInt;
        case AttributeFormat::Int2:
            return MTLVertexFormatInt2;
        case AttributeFormat::Int3:
            return MTLVertexFormatInt3;
        case AttributeFormat::Int4:
            return MTLVertexFormatInt4;

        case AttributeFormat::UInt:
            return MTLVertexFormatUInt;
        case AttributeFormat::UInt2:
            return MTLVertexFormatUInt2;
        case AttributeFormat::UInt3:
            return MTLVertexFormatUInt3;
        case AttributeFormat::UInt4:
            return MTLVertexFormatUInt4;

        case AttributeFormat::Byte:
            return MTLVertexFormatChar;
        case AttributeFormat::Byte2:
            return MTLVertexFormatChar2;
        case AttributeFormat::Byte3:
            return MTLVertexFormatChar3;
        case AttributeFormat::Byte4:
            return MTLVertexFormatChar4;

        case AttributeFormat::UByte:
            return MTLVertexFormatUChar;
        case AttributeFormat::UByte2:
            return MTLVertexFormatUChar2;
        case AttributeFormat::UByte3:
            return MTLVertexFormatUChar3;
        case AttributeFormat::UByte4:
            return MTLVertexFormatUChar4;
 
        default:
            FF_CONSOLE_ERROR("Unable to convert attribute format to metal format.");
        }
    }
    
    MetalVertexDescriptor::MetalVertexDescriptor(MetalGraphicsDevice* const& graphicsDevice,
        const VertexDescriptorInfo& info)
        :IVertexDescriptor(info) {
        _vertexDescriptor = [MTLVertexDescriptor vertexDescriptor];
        for(BufferDescriptorInfo const& buffer : info.buffers) {
            _vertexDescriptor.layouts[METAL_MAX_BUFFER_COUNT - buffer.slot].stride = buffer.stride;
            // I'm not sure if OpenGL/DirectX even support different
            // step functions (haven't looked into instancing on OpenGL).
            // If so this may need to be modified. Vulkan/D3D12 definitely
            // can support this, but I probably won't wrap an abstraction
            // around those in the near future.
            _vertexDescriptor.layouts[METAL_MAX_BUFFER_COUNT - buffer.slot].stepFunction = MTLVertexStepFunctionPerVertex;

            for(AttributeDescriptorInfo const& attribute : buffer.attributes) {
                _vertexDescriptor.attributes[attribute.location].offset = attribute.offset;
                _vertexDescriptor.attributes[attribute.location].bufferIndex = METAL_MAX_BUFFER_COUNT - buffer.slot;
                _vertexDescriptor.attributes[attribute.location].format = convertAttributeFormatToMetalFormat(attribute.format);
            }
        }
    }
    MetalVertexDescriptor::~MetalVertexDescriptor() {
    }

    MTLVertexDescriptor* MetalVertexDescriptor::getVertexDescriptor() const {
        return _vertexDescriptor;
    }
}
