/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-metal/MetalBlitPass.hmm>

#include <ff-graphics-metal/MetalTextureImp.hmm>

namespace ff {

MetalBlitPass::MetalBlitPass(MetalGraphicsDevice* const& graphicsDevice)
    :_graphicsDevice(graphicsDevice) {
    _commandEncoder = [graphicsDevice->getCurrentCommandBuffer() blitCommandEncoder];
}

void MetalBlitPass::genMipMapsImpl(ResourceHandle<ColorTexture> const& texture) {
    MetalTextureImp* metalTextureImp = static_cast<MetalTextureImp*>(texture->getImp());
    [_commandEncoder generateMipmapsForTexture:metalTextureImp->getMTLTexture()];
}

void MetalBlitPass::endImpl() {
    [_commandEncoder endEncoding];
}

}
