/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-metal/MetalRenderState.hmm>

#include <ff-graphics-metal/MetalBufferFormats.hmm>
#include <ff-graphics-metal/MetalBlendStateHelpers.hmm>
#include <ff-graphics-metal/MetalVertexDescriptor.hmm>
#include <ff-graphics-metal/MetalShader.hmm>
#include <memory>

namespace ff {
    MetalRenderState::MetalRenderState(MetalGraphicsDevice* const& metalGraphicsDevice,
        std::shared_ptr<IShader> const& shader,
        std::shared_ptr<IVertexDescriptor> const& vertexDescriptor,
        std::vector<RenderColorLayerState> const& renderColorLayerStates,
        std::optional<DepthBufferFormat> const& depthBufferFormat,
        const std::vector<MTLPixelFormat>& colorBufferOverrides)
        :IRenderState(shader,
            vertexDescriptor,
            renderColorLayerStates,
            depthBufferFormat),
        _graphicsDevice(metalGraphicsDevice) {
        MTLRenderPipelineDescriptor* descriptor = [MTLRenderPipelineDescriptor new];

        std::vector<RenderColorLayerState> colorLayerStates = getRenderColorLayerStates();
        for(int i = 0; i < colorLayerStates.size(); i++) {
            RenderColorLayerState const& colorLayerState = colorLayerStates[i];

            descriptor.colorAttachments[i].pixelFormat = convertBufferFormatToMetalPixelFormat(colorLayerState.colorBufferFormat);
            if(i < colorBufferOverrides.size()) {
                descriptor.colorAttachments[i].pixelFormat = colorBufferOverrides[i];
            }

            descriptor.colorAttachments[i].blendingEnabled = colorLayerState.blendState != std::nullopt;
            if(colorLayerState.blendState) {
                BlendState const& blendState = *colorLayerState.blendState;

                descriptor.colorAttachments[i].rgbBlendOperation = convertBlendOperationToMetalBlendOperation(blendState.rgbBlendOperation);
                descriptor.colorAttachments[i].alphaBlendOperation = convertBlendOperationToMetalBlendOperation(blendState.alphaBlendOperation);

                descriptor.colorAttachments[i].sourceRGBBlendFactor = convertBlendFactorToMetalBlendFactor(blendState.rgbSourceBlendFactor);
                descriptor.colorAttachments[i].sourceAlphaBlendFactor = convertBlendFactorToMetalBlendFactor(blendState.alphaSourceBlendFactor);

                descriptor.colorAttachments[i].destinationRGBBlendFactor = convertBlendFactorToMetalBlendFactor(blendState.rgbDestBlendFactor);
                descriptor.colorAttachments[i].destinationAlphaBlendFactor = convertBlendFactorToMetalBlendFactor(blendState.alphaDestBlendFactor);
            }
        }

        if(depthBufferFormat != std::nullopt) {
            descriptor.depthAttachmentPixelFormat = convertBufferFormatToMetalPixelFormat(*depthBufferFormat);
        }

        MetalShader& metalShader = static_cast<MetalShader&>(*getShader());

        descriptor.vertexFunction = metalShader.getVertexFunction();
        descriptor.fragmentFunction = metalShader.getFragmentFunction();

        MetalVertexDescriptor& metalVertexDescriptor = static_cast<MetalVertexDescriptor&>(*getVertexDescriptor());
        descriptor.vertexDescriptor = metalVertexDescriptor.getVertexDescriptor();

        NSError* error;
        _renderState = [_graphicsDevice->getDevice() newRenderPipelineStateWithDescriptor: descriptor error:&error];
        FF_ASSERT(_renderState, "Could not create render pipeline state (%s).", [[error description] UTF8String]);
    }

    id<MTLRenderPipelineState> MetalRenderState::getMetalRenderPipelineState() {
        return _renderState; 
    }
}
