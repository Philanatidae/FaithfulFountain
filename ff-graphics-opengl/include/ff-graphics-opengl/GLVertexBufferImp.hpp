/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_VERTEX_BUFFER_IMP_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_VERTEX_BUFFER_IMP_HPP

#include <ff-graphics-opengl/GLBuffer.hpp>
#include <ff/graphics/VertexBuffer.hpp>

namespace ff {
    class GLVertexBufferImp : public VertexBufferImp {
    public:
        GLVertexBufferImp(GLGraphicsDevice* const& graphicsDevice, int const& elementSize, int const& count);
        ~GLVertexBufferImp() = default;

        void bufferData(void const* const& ptr, int const& size, int const& start) override;

        void bind();

        GLuint getGLBuffer() const;

    private:
        GLBuffer _buffer;
    };
}

#endif
