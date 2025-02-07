/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_UNIFORM_BUFFER_IMP_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_UNIFORM_BUFFER_IMP_HPP

#include <ff-graphics-opengl/GLBuffer.hpp>
#include <ff/graphics/UniformBuffer.hpp>

namespace ff {
    class GLUniformBufferImp : public UniformBufferImp {
    public:
        GLUniformBufferImp(GLGraphicsDevice* const& graphicsDevice, int const& elementSize);
        ~GLUniformBufferImp() = default;

        void bufferData(void const* const& ptr, int const& size, int const& start) override;

        void bindToLocation(const int& slot);
    private:
        GLBuffer _buffer;
    };
}

#endif
