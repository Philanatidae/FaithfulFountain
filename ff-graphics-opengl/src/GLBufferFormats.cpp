/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLBufferFormats.hpp>

#include <ff/Console.hpp>

namespace ff {
    GLenum convertBufferFormatToGLSizedPixelFormat(ColorBufferFormat const& format) {
        switch(format) {
            case ColorBufferFormat::RGBA8Unorm:
                return GL_RGBA8;
            case ColorBufferFormat::RGBA32Uint:
                return GL_RGBA32UI;
            case ColorBufferFormat::RG32Uint:
                return GL_RG32UI;
            case ColorBufferFormat::Invalid:
            default:
                FF_CONSOLE_FATAL("Unknown conversion from color buffer format to OpenGL.");
                return -1;
        }
    }
    GLenum convertBufferFormatToGLSizedPixelFormat(DepthBufferFormat const& format) {
        switch(format) {
            case DepthBufferFormat::D32Float:
                return GL_DEPTH_COMPONENT32F;

            case DepthBufferFormat::Invalid:
            default:
                FF_CONSOLE_FATAL("Unknown conversion from depth buffer format to OpenGL.");
                return -1;
        }
    }
    std::pair<GLenum, GLenum> convertGLSizedPixelFormatToGLPixelFormatAndType(GLenum const& format) {
        // https://gist.github.com/Kos/4739337
        // @todo Add all format pairs

        switch(format) {
        // COLOR //
        case GL_RGBA8:
            return std::make_pair(GL_RGBA, GL_UNSIGNED_BYTE);

        case GL_RG32UI:
            return std::make_pair(GL_RG_INTEGER, GL_UNSIGNED_INT);

        case GL_RGBA32UI:
            return std::make_pair(GL_RGBA_INTEGER, GL_UNSIGNED_INT);

        // DEPTH // 
        case GL_DEPTH_COMPONENT32F:
            return std::make_pair(GL_DEPTH_COMPONENT, GL_FLOAT);

        default:
            FF_CONSOLE_FATAL("Format conversion not yet supported.");
            return std::make_pair(-1, -1);
        }
    }
}
