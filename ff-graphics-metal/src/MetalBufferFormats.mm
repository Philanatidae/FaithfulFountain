/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-metal/MetalBufferFormats.hmm>

#include <ff/Console.hpp>

namespace ff {
    MTLPixelFormat convertBufferFormatToMetalPixelFormat(const ColorBufferFormat& filter) {
        switch(filter) {
            case ColorBufferFormat::RGBA8Unorm:
                return MTLPixelFormatRGBA8Unorm;

            case ColorBufferFormat::RGBA32Uint:
                return MTLPixelFormatRGBA32Uint;

            case ColorBufferFormat::RG32Uint:
                return MTLPixelFormatRG32Uint;

            case ColorBufferFormat::Invalid:
            default:
                return MTLPixelFormatInvalid;
        }
    }

    MTLPixelFormat convertBufferFormatToMetalPixelFormat(const DepthBufferFormat& filter) {
        switch(filter) {
            case DepthBufferFormat::D32Float:
                return MTLPixelFormatDepth32Float;

            case DepthBufferFormat::Invalid:
            default:
                return MTLPixelFormatInvalid;
        }
    }

    void checkMetalPixelFormatRecognized(MTLPixelFormat format) {
        switch(format) {
        case MTLPixelFormatA8Unorm:
        case MTLPixelFormatR8Unorm:
        case MTLPixelFormatR8Unorm_sRGB:
        case MTLPixelFormatR8Snorm:
        case MTLPixelFormatR8Uint:
        case MTLPixelFormatR8Sint:
        case MTLPixelFormatR16Unorm:
        case MTLPixelFormatR16Snorm:
        case MTLPixelFormatR16Uint:
        case MTLPixelFormatR16Sint:
        case MTLPixelFormatR16Float:
        case MTLPixelFormatRG8Unorm:
        case MTLPixelFormatRG8Unorm_sRGB:
        case MTLPixelFormatRG8Snorm:
        case MTLPixelFormatRG8Uint:
        case MTLPixelFormatRG8Sint:
        case MTLPixelFormatB5G6R5Unorm:
        case MTLPixelFormatA1BGR5Unorm:
        case MTLPixelFormatABGR4Unorm:
        case MTLPixelFormatBGR5A1Unorm:
        case MTLPixelFormatR32Uint:
        case MTLPixelFormatR32Sint:
        case MTLPixelFormatR32Float:
        case MTLPixelFormatRG16Unorm:
        case MTLPixelFormatRG16Snorm:
        case MTLPixelFormatRG16Uint:
        case MTLPixelFormatRG16Sint:
        case MTLPixelFormatRG16Float:
        case MTLPixelFormatRGBA8Unorm:
        case MTLPixelFormatRGBA8Unorm_sRGB:
        case MTLPixelFormatRGBA8Snorm:
        case MTLPixelFormatRGBA8Uint:
        case MTLPixelFormatRGBA8Sint:
        case MTLPixelFormatBGRA8Unorm:
        case MTLPixelFormatBGRA8Unorm_sRGB:
        case MTLPixelFormatRGB10A2Unorm:
        case MTLPixelFormatRGB10A2Uint:
        case MTLPixelFormatRG11B10Float:
        case MTLPixelFormatRGB9E5Float:
        case MTLPixelFormatBGR10A2Unorm:
        case MTLPixelFormatBGR10_XR:
        case MTLPixelFormatBGR10_XR_sRGB:
        case MTLPixelFormatRG32Uint:
        case MTLPixelFormatRG32Sint:
        case MTLPixelFormatRG32Float:
        case MTLPixelFormatRGBA16Unorm:
        case MTLPixelFormatRGBA16Snorm:
        case MTLPixelFormatRGBA16Uint:
        case MTLPixelFormatRGBA16Sint:
        case MTLPixelFormatRGBA16Float:
        case MTLPixelFormatBGRA10_XR:
        case MTLPixelFormatBGRA10_XR_sRGB:
        case MTLPixelFormatRGBA32Uint:
        case MTLPixelFormatRGBA32Sint:
        case MTLPixelFormatRGBA32Float:
        case MTLPixelFormatBC1_RGBA:
        case MTLPixelFormatBC1_RGBA_sRGB:
        case MTLPixelFormatBC2_RGBA:
        case MTLPixelFormatBC2_RGBA_sRGB:
        case MTLPixelFormatBC3_RGBA:
        case MTLPixelFormatBC3_RGBA_sRGB:
        case MTLPixelFormatBC4_RUnorm:
        case MTLPixelFormatBC4_RSnorm:
        case MTLPixelFormatBC5_RGUnorm:
        case MTLPixelFormatBC5_RGSnorm:
        case MTLPixelFormatBC6H_RGBFloat:
        case MTLPixelFormatBC6H_RGBUfloat:
        case MTLPixelFormatBC7_RGBAUnorm:
        case MTLPixelFormatBC7_RGBAUnorm_sRGB:
        case MTLPixelFormatPVRTC_RGB_2BPP:
        case MTLPixelFormatPVRTC_RGB_2BPP_sRGB:
        case MTLPixelFormatPVRTC_RGB_4BPP:
        case MTLPixelFormatPVRTC_RGB_4BPP_sRGB:
        case MTLPixelFormatPVRTC_RGBA_2BPP:
        case MTLPixelFormatPVRTC_RGBA_2BPP_sRGB:
        case MTLPixelFormatPVRTC_RGBA_4BPP:
        case MTLPixelFormatPVRTC_RGBA_4BPP_sRGB:
        case MTLPixelFormatEAC_R11Unorm:
        case MTLPixelFormatEAC_R11Snorm:
        case MTLPixelFormatEAC_RG11Unorm:
        case MTLPixelFormatEAC_RG11Snorm:
        case MTLPixelFormatEAC_RGBA8:
        case MTLPixelFormatEAC_RGBA8_sRGB:
        case MTLPixelFormatETC2_RGB8:
        case MTLPixelFormatETC2_RGB8_sRGB:
        case MTLPixelFormatETC2_RGB8A1:
        case MTLPixelFormatETC2_RGB8A1_sRGB:
        case MTLPixelFormatASTC_4x4_sRGB:
        case MTLPixelFormatASTC_5x4_sRGB:
        case MTLPixelFormatASTC_5x5_sRGB:
        case MTLPixelFormatASTC_6x5_sRGB:
        case MTLPixelFormatASTC_6x6_sRGB:
        case MTLPixelFormatASTC_8x5_sRGB:
        case MTLPixelFormatASTC_8x6_sRGB:
        case MTLPixelFormatASTC_8x8_sRGB:
        case MTLPixelFormatASTC_10x5_sRGB:
        case MTLPixelFormatASTC_10x6_sRGB:
        case MTLPixelFormatASTC_10x8_sRGB:
        case MTLPixelFormatASTC_10x10_sRGB:
        case MTLPixelFormatASTC_12x10_sRGB:
        case MTLPixelFormatASTC_12x12_sRGB:
        case MTLPixelFormatASTC_4x4_LDR:
        case MTLPixelFormatASTC_5x4_LDR:
        case MTLPixelFormatASTC_5x5_LDR:
        case MTLPixelFormatASTC_6x5_LDR:
        case MTLPixelFormatASTC_6x6_LDR:
        case MTLPixelFormatASTC_8x5_LDR:
        case MTLPixelFormatASTC_8x6_LDR:
        case MTLPixelFormatASTC_8x8_LDR:
        case MTLPixelFormatASTC_10x5_LDR:
        case MTLPixelFormatASTC_10x6_LDR:
        case MTLPixelFormatASTC_10x8_LDR:
        case MTLPixelFormatASTC_10x10_LDR:
        case MTLPixelFormatASTC_12x10_LDR:
        case MTLPixelFormatASTC_12x12_LDR:
        case MTLPixelFormatASTC_4x4_HDR:
        case MTLPixelFormatASTC_5x4_HDR:
        case MTLPixelFormatASTC_5x5_HDR:
        case MTLPixelFormatASTC_6x5_HDR:
        case MTLPixelFormatASTC_6x6_HDR:
        case MTLPixelFormatASTC_8x5_HDR:
        case MTLPixelFormatASTC_8x6_HDR:
        case MTLPixelFormatASTC_8x8_HDR:
        case MTLPixelFormatASTC_10x5_HDR:
        case MTLPixelFormatASTC_10x6_HDR:
        case MTLPixelFormatASTC_10x8_HDR:
        case MTLPixelFormatASTC_10x10_HDR:
        case MTLPixelFormatASTC_12x10_HDR:
        case MTLPixelFormatASTC_12x12_HDR:
        case MTLPixelFormatGBGR422:
        case MTLPixelFormatBGRG422:
        case MTLPixelFormatDepth16Unorm:
        case MTLPixelFormatDepth32Float:
        case MTLPixelFormatStencil8:
        case MTLPixelFormatDepth24Unorm_Stencil8:
        case MTLPixelFormatDepth32Float_Stencil8:
        case MTLPixelFormatX32_Stencil8:
        case MTLPixelFormatX24_Stencil8:
            return;
        default:
            FF_CONSOLE_FATAL("Metal pixel format is not recognized: %s", format);
        }
    }
    bool isMetalPixelFormatColor(MTLPixelFormat format) {
        checkMetalPixelFormatRecognized(format);

        switch(format) {
        case MTLPixelFormatDepth16Unorm:
        case MTLPixelFormatDepth32Float:
        case MTLPixelFormatStencil8:
        case MTLPixelFormatDepth24Unorm_Stencil8:
        case MTLPixelFormatDepth32Float_Stencil8:
        case MTLPixelFormatX32_Stencil8:
        case MTLPixelFormatX24_Stencil8:
            return false;
        default:
            return true;
        }
    }
}
