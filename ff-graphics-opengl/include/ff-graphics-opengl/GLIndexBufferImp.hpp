/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_INDEX_BUFFER_IMP_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_INDEX_BUFFER_IMP_HPP

#include <ff-graphics-opengl/GLBuffer.hpp>
#include <ff/graphics/IndexBuffer.hpp>

namespace ff {
    class GLIndexBufferImp : public IndexBufferImp {
    public:
        GLIndexBufferImp(GLGraphicsDevice* const& graphicsDevice, int const& elementSize, int const& count);
        ~GLIndexBufferImp() = default;

        void bufferData(void const* const& data, const int& size, const int& start) override;

        void bind();

        GLuint getGLBuffer() const;

    private:
        GLBuffer _buffer;
    };
}

#endif
