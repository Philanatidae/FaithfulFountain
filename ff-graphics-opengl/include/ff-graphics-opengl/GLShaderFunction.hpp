/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_SHADER_FUNCTION_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_SHADER_FUNCTION_HPP

#include <ff-graphics-opengl/gl.hpp>
#include <ff-graphics-opengl/GLGraphicsDevice.hpp>
#include <string>

namespace ff {
    class GLShaderFunction final {
    public:
        GLShaderFunction(GLGraphicsDevice* const& graphicsDevice, const std::string& name, const GLenum& shaderType, const std::string& source);
        ~GLShaderFunction();

        void attachToShaderProgram(const GLuint& program);
        void detatchFromShaderProgram(const GLuint& program);

        const std::string& getName() const;
    private:
        std::string const _name;
        GLuint _shader;
    };
}

#endif
