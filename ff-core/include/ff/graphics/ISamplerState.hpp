/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_ISAMPLER_STATE_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_ISAMPLER_STATE_HPP

namespace ff {
    enum class SamplerFilter {
        LINEAR,
        NEAREST
    };
    enum class SamplerMipFilter {
        INVALID,
        LINEAR,
        NEAREST
    };

    enum class SamplerAddressMode {
        CLAMP_TO_EDGE,
        /*CLAMP_TO_ZERO,*/ // Not supported by OpenGL ES 3.0; clamp to color of (0, 0, 0)?
        REPEAT,
        REPEAT_MIRROR
    };

    class ISamplerState {
    public:
        ISamplerState(SamplerFilter const& minFilter,
            SamplerFilter const& magFilter,
            SamplerAddressMode const& sAddressMode,
            SamplerAddressMode const& tAddressMode,
            SamplerMipFilter const& mipFilter);
        virtual ~ISamplerState() = default;

        SamplerFilter getMinFilter() const;
        SamplerFilter getMagFilter() const;
        SamplerMipFilter getMipFilter() const;

        SamplerAddressMode getSAddressMode() const;
        SamplerAddressMode getTAddressMode() const;

    private:
        SamplerFilter _minFilter;
        SamplerFilter _magFilter;
        SamplerAddressMode _sAddressMode;
        SamplerAddressMode _tAddressMode;
        SamplerMipFilter _mipFilter;
    };

    class NullSamplerState : public ISamplerState {
    public:
        NullSamplerState(const SamplerFilter& minFilter,
            const SamplerFilter& magFilter,
            const SamplerAddressMode& sAddressMode,
            const SamplerAddressMode& tAddressMode,
            const SamplerMipFilter& mipFilter = SamplerMipFilter::INVALID)
            :ISamplerState(minFilter, magFilter, sAddressMode, tAddressMode, mipFilter) { }
        virtual ~NullSamplerState() = default;
    };
}

#endif
