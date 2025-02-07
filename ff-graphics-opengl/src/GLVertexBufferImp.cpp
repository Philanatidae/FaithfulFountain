/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLVertexBufferImp.hpp>

namespace ff {
    GLVertexBufferImp::GLVertexBufferImp(GLGraphicsDevice* const& graphicsDevice, int const& elementSize, int const& count)
        :_buffer(graphicsDevice, GL_ARRAY_BUFFER, elementSize, count) {
    }

    void GLVertexBufferImp::bufferData(void const* const& ptr, int const& size, int const& start) {
        _buffer.bufferData(ptr, size, start);
    }

    void GLVertexBufferImp::bind() {
        _buffer.bind();
    }

    GLuint GLVertexBufferImp::getGLBuffer() const {
        return _buffer.getGLBuffer();
    }
}
