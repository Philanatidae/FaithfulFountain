/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_BLEND_STATE_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_BLEND_STATE_HPP

namespace ff {
    enum class BlendOperation {
        Invalid,

        Add,
        Subtract,
        ReverseSubtract,
        Min,
        Max
    };

    enum class BlendFactor {
        Invalid,

        Zero,
        One,

        Source,
        OneMinusSource,

        SourceAlpha,
        OneMinusSourceAlpha,

        Dest,
        OneMinusDest,

        DestAlpha,
        OneMinusDestAlpha
    };

    struct BlendState {
        BlendState(const BlendOperation& rgbBlendOperation,
            const BlendFactor& rgbSourceBlendFactor,
            const BlendFactor& rgbDestBlendFactor,
            
            const BlendOperation& alphaBlendOperation,
            const BlendFactor& alphaSourceBlendFactor,
            const BlendFactor& alphaDestBlendFactor)
            :rgbBlendOperation(rgbBlendOperation),
            rgbSourceBlendFactor(rgbSourceBlendFactor),
            rgbDestBlendFactor(rgbDestBlendFactor),
            
            alphaBlendOperation(alphaBlendOperation),
            alphaSourceBlendFactor(alphaSourceBlendFactor),
            alphaDestBlendFactor(alphaDestBlendFactor) {
        }

        BlendOperation rgbBlendOperation;
        BlendFactor rgbSourceBlendFactor;
        BlendFactor rgbDestBlendFactor;

        BlendOperation alphaBlendOperation;
        BlendFactor alphaSourceBlendFactor;
        BlendFactor alphaDestBlendFactor;

        bool operator==(BlendState const& other) const {
            return rgbBlendOperation == other.rgbBlendOperation
                && rgbSourceBlendFactor == other.rgbSourceBlendFactor
                && rgbDestBlendFactor == other.rgbDestBlendFactor
                && alphaBlendOperation == other.alphaBlendOperation
                && alphaSourceBlendFactor == other.alphaSourceBlendFactor
                && alphaDestBlendFactor == other.alphaDestBlendFactor;
        }
        bool operator!=(BlendState const& other) const {
            return !(*this == other);
        }
    };
}

#endif
