/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "ff/Console.hpp"
#include "ff/graphics/ISamplerState.hpp"
#include <ff-graphics-metal/MetalSamplerState.hmm>

namespace ff {
    MTLSamplerMinMagFilter convertMinMagFilterToMetalFilter(const SamplerFilter& filter) {
        switch(filter) {
            case SamplerFilter::NEAREST:
                return MTLSamplerMinMagFilterNearest;
            case SamplerFilter::LINEAR:
                return MTLSamplerMinMagFilterLinear;
            default:
                FF_CONSOLE_ERROR("Unknown conversion of sampler filter to Metal sampler filter.");
        }
    }
    MTLSamplerAddressMode convertAddressModeToMetalAddressMode(const SamplerAddressMode& addressMode) {
        switch(addressMode) {
            case SamplerAddressMode::REPEAT_MIRROR:
                return MTLSamplerAddressModeMirrorRepeat;
            case SamplerAddressMode::REPEAT:
                return MTLSamplerAddressModeRepeat;
            case SamplerAddressMode::CLAMP_TO_EDGE:
                return MTLSamplerAddressModeClampToEdge;
            default:
                FF_CONSOLE_ERROR("Unknown conversion of sampler address mode to Metal sampler address mode.");
            // case SamplerAddressMode::CLAMP_TO_ZERO:
            //     return MTLSamplerAddressModeClampToZero;
        }
    }
    MTLSamplerMipFilter convertMipFilterToMetalFilter(const SamplerMipFilter& filter) {
        switch(filter) {
            case SamplerMipFilter::INVALID:
                return MTLSamplerMipFilterNotMipmapped;
            case SamplerMipFilter::NEAREST:
                return MTLSamplerMipFilterNearest;
            case SamplerMipFilter::LINEAR:
                return MTLSamplerMipFilterLinear;
            default:
                FF_CONSOLE_ERROR("Unknown conversion of mip filter to Metal mip filter.");
        }
    }

    MetalSamplerState::MetalSamplerState(MetalGraphicsDevice* const& graphicsDevice,
        const SamplerFilter& minFilter,
        const SamplerFilter& magFilter,
        const SamplerAddressMode& sAddressMode,
        const SamplerAddressMode& tAddressMode,
        const SamplerMipFilter& mipFilter)
        :ISamplerState(minFilter, magFilter, sAddressMode, tAddressMode, mipFilter) {
        MTLSamplerDescriptor* samplerDescriptor = [MTLSamplerDescriptor new];
        samplerDescriptor.minFilter = convertMinMagFilterToMetalFilter(minFilter);
        samplerDescriptor.magFilter = convertMinMagFilterToMetalFilter(magFilter);
        samplerDescriptor.mipFilter = convertMipFilterToMetalFilter(mipFilter);

        samplerDescriptor.sAddressMode = convertAddressModeToMetalAddressMode(sAddressMode);
        samplerDescriptor.tAddressMode = convertAddressModeToMetalAddressMode(tAddressMode);

        _samplerState = [graphicsDevice->getDevice() newSamplerStateWithDescriptor:samplerDescriptor];
    }
    MetalSamplerState::~MetalSamplerState() {
    }

    id<MTLSamplerState> MetalSamplerState::getSamplerState() const {
        return _samplerState;
    }
}
