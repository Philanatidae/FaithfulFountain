/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLUniformBufferImp.hpp>

namespace ff {
    GLUniformBufferImp::GLUniformBufferImp(GLGraphicsDevice* const& graphicsDevice, int const& elementSize)
        :_buffer(graphicsDevice, GL_UNIFORM_BUFFER, elementSize, 1) {
    }

    void GLUniformBufferImp::bufferData(void const* const& ptr, int const& size, int const& start) {
        _buffer.bufferData(ptr, size, start);
    }

    void GLUniformBufferImp::bindToLocation(const int& slot) {
        _buffer.bind();
        FF_GL_CALL(glBindBufferBase, GL_UNIFORM_BUFFER,
            slot,
            _buffer.getGLBuffer());
    }
}
