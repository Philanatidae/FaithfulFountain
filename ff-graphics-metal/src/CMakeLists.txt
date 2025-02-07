# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

target_sources(ff-graphics-metal PRIVATE
    MetalBlendStateHelpers.mm
    MetalBlitPass.mm
    MetalBuffer.mm
    MetalBufferFormats.mm
    MetalDepthStencilState.mm
    MetalGraphicsDevice.mm
    MetalLibrary.mm
    MetalRenderPass.mm
    MetalRenderState.mm
    MetalSamplerState.mm
    MetalShader.mm
    MetalShaderLibrary.mm
    MetalTextureImp.mm
    MetalVertexDescriptor.mm
)
