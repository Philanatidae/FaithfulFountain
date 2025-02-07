/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/BufferFormats.hpp>

#include <stdint.h>
#include <numeric>

#include <ff/Console.hpp>

namespace ff {

BufferFormatInfo_t BufferFormatInfo::get(ColorBufferFormat const& format) {
    BufferFormatInfo_t formatInfo;
    std::memset(&formatInfo, 0, sizeof(BufferFormatInfo_t));
    formatInfo.baseType = FormatBaseType::Invalid;
    formatInfo.name = "Invalid";

    switch(format) {
        case ColorBufferFormat::RGBA8Unorm:
            formatInfo.componentSize = 1;
            formatInfo.componentCount = 4;
            formatInfo.name = "RGBA8Unorm";
            formatInfo.baseType = FormatBaseType::FLOAT;
            formatInfo.clearMin = 0;
            formatInfo.clearMax = 1;
            break;
        case ColorBufferFormat::RGBA32Uint:
            formatInfo.componentSize = 4;
            formatInfo.componentCount = 4;
            formatInfo.name = "RGBA32Uint";
            formatInfo.baseType = FormatBaseType::UINT;
            formatInfo.clearMin = 0;
            formatInfo.clearMax = std::numeric_limits<uint32_t>::max();
            break;

        case ColorBufferFormat::RG32Uint:
            formatInfo.componentSize = 4;
            formatInfo.componentCount = 2;
            formatInfo.name = "RG32Uint";
            formatInfo.baseType = FormatBaseType::UINT;
            formatInfo.clearMin = 0;
            formatInfo.clearMax = std::numeric_limits<uint32_t>::max();
            break;

        default:
            FF_CONSOLE_FATAL("Unimplemented format.");
            break;
    }

    formatInfo.size = formatInfo.componentSize * formatInfo.componentCount;

    return formatInfo;
}
BufferFormatInfo_t BufferFormatInfo::get(DepthBufferFormat const& format) {
    BufferFormatInfo_t formatInfo;
    std::memset(&formatInfo, 0, sizeof(BufferFormatInfo_t));
    formatInfo.baseType = FormatBaseType::Invalid;
    formatInfo.name = "Invalid";

    switch(format) {
        case DepthBufferFormat::D32Float:
            formatInfo.componentSize = 4;
            formatInfo.componentCount = 1;
            formatInfo.name = "D32Float";
            formatInfo.baseType = FormatBaseType::FLOAT;
            formatInfo.clearMin = 0;
            formatInfo.clearMax = 1;
            break;

        default:
            FF_CONSOLE_FATAL("Unimplemented format.");
            break;
    }

    formatInfo.size = formatInfo.componentSize * formatInfo.componentCount;

    return formatInfo;
}
BufferFormatInfo_t BufferFormatInfo::get(TextureFormat const& format) {
    switch(format) {
        case TextureFormat::RGBA8Unorm:
            return BufferFormatInfo::get(ColorBufferFormat::RGBA8Unorm);
        case TextureFormat::RGBA32Uint:
            return BufferFormatInfo::get(ColorBufferFormat::RGBA32Uint);

        case TextureFormat::RG32Uint:
            return BufferFormatInfo::get(ColorBufferFormat::RG32Uint);

        case TextureFormat::D32Float:
            return BufferFormatInfo::get(DepthBufferFormat::D32Float);

        default:
        {
            BufferFormatInfo_t formatInfo;
            std::memset(&formatInfo, 0, sizeof(BufferFormatInfo_t));
            formatInfo.baseType = FormatBaseType::Invalid;
            formatInfo.name = "Invalid";
            return formatInfo;
        }
    }
}

bool BufferFormatInfo::matches(TextureFormat const& formatT,
    ColorBufferFormat const& formatC) {
    return convertTextureFormatToColorBufferFormat(formatT) == formatC;
}
bool BufferFormatInfo::matches(ColorBufferFormat const& formatC,
    TextureFormat const& formatT) {
    return convertTextureFormatToColorBufferFormat(formatT) == formatC;
}
bool BufferFormatInfo::matches(TextureFormat const& formatT,
    DepthBufferFormat const& formatD) {
    return ff::convertTextureFormatToDepthBufferFormat(formatT) == formatD;
}
bool BufferFormatInfo::matches(DepthBufferFormat const& formatD,
    TextureFormat const& formatT) {
    return ff::convertTextureFormatToDepthBufferFormat(formatT) == formatD;
}

ColorBufferFormat convertTextureFormatToColorBufferFormat(TextureFormat const& format) {
        switch(format) {
        case TextureFormat::RGBA8Unorm:
            return ColorBufferFormat::RGBA8Unorm;
        case TextureFormat::RGBA32Uint:
            return ColorBufferFormat::RGBA32Uint;

        case TextureFormat::RG32Uint:
            return ColorBufferFormat::RG32Uint;

        default:
            return ColorBufferFormat::Invalid;
        }
    }
    DepthBufferFormat convertTextureFormatToDepthBufferFormat(TextureFormat const& format) {
        switch(format) {
        case TextureFormat::D32Float:
            return DepthBufferFormat::D32Float;

        default:
            return DepthBufferFormat::Invalid;
        }
    }

}
