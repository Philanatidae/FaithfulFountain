/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLIndexBufferImp.hpp>

namespace ff {
    GLIndexBufferImp::GLIndexBufferImp(GLGraphicsDevice* const& graphicsDevice, int const& elementSize, int const& count)
        :_buffer(graphicsDevice, GL_ELEMENT_ARRAY_BUFFER, elementSize, count) {
    }

    void GLIndexBufferImp::bufferData(void const* const& data, const int& size, const int& start) {
        _buffer.bufferData(data, size, start);
    }

    void GLIndexBufferImp::bind() {
        _buffer.bind();
    }

    GLuint GLIndexBufferImp::getGLBuffer() const {
        return _buffer.getGLBuffer();
    }
}
