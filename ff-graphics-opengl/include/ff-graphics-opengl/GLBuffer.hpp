/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_BUFFER_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_BUFFER_HPP

#include <stdio.h>

#include <ff-graphics-opengl/gl.hpp>
#include <ff-graphics-opengl/GLGraphicsDevice.hpp>

namespace ff {
    class GLBuffer {
    public:
        GLBuffer(GLGraphicsDevice* const& graphicsDevice, GLenum target, size_t elementSize, size_t count);
        ~GLBuffer();

        const size_t& getElementSize() const;
        const size_t& getTotalSize() const;
        size_t getCount() const;

        void bufferData(void const* ptr, const size_t& count, const size_t& start = 0);

        void bind();

        GLuint getGLBuffer() const;

    private:
        GLGraphicsDevice* const _graphicsDevice;

        size_t const _elementSize;
        size_t const _totalSize;

        GLenum _target;
        GLuint _buffer;
        bool _hasBoundBuffer;
    };
}

#endif
