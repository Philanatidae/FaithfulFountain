/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_BUFFER_FORMATS_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_BUFFER_FORMATS_HPP

#include <stdint.h>
#include <stdio.h>

namespace ff {
    enum FormatBaseType {
        Invalid = -1,

        INT = 0,
        UINT,
        FLOAT,
        FLOAT_INT
    };

    enum class ColorBufferFormat {
        Invalid = -1,
        Native = 0,
        RGBA8Unorm,
        RGBA32Uint,
        RG32Uint
    };
    enum class DepthBufferFormat {
        Invalid = -1,
        D32Float = 0
    };

    enum class TextureFormat {
        Invalid = -1,

        RGBA8Unorm = 0,
        RGBA32Uint,

        RG32Uint,

        D32Float
    };

    struct BufferFormatInfo_t {
        size_t componentSize;
        size_t componentCount;
        size_t size;
        char const* name;
        FormatBaseType baseType;
        float clearMax;
        float clearMin;
    };

    struct BufferFormatInfo {
        static BufferFormatInfo_t get(ColorBufferFormat const& format);
        static BufferFormatInfo_t get(DepthBufferFormat const& format);
        static BufferFormatInfo_t get(TextureFormat const& format);

        static bool matches(TextureFormat const& formatT,
            ColorBufferFormat const& formatC);
        static bool matches(ColorBufferFormat const& formatC,
            TextureFormat const& formatT); 
        static bool matches(TextureFormat const& formatT,
            DepthBufferFormat const& formatD);
        static bool matches(DepthBufferFormat const& formatD,
            TextureFormat const& formatT);
    };

    ColorBufferFormat convertTextureFormatToColorBufferFormat(TextureFormat const& format);
    DepthBufferFormat convertTextureFormatToDepthBufferFormat(TextureFormat const& format);
}

#endif
