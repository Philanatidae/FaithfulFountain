/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_BUFFER_FORMATS_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_BUFFER_FORMATS_HPP

#include <ff/graphics/BufferFormats.hpp>
#include <ff-graphics-opengl/gl.hpp>
#include <utility>

namespace ff {
    GLenum convertBufferFormatToGLSizedPixelFormat(ColorBufferFormat const& format);
    GLenum convertBufferFormatToGLSizedPixelFormat(DepthBufferFormat const& format);

    std::pair<GLenum, GLenum> convertGLSizedPixelFormatToGLPixelFormatAndType(GLenum const& format);
}

#endif
