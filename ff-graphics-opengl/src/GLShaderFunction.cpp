/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLShaderFunction.hpp>

#include <ff/Console.hpp>
#include <vector>

namespace ff {
    GLShaderFunction::GLShaderFunction(GLGraphicsDevice* const& graphicsDevice, const std::string& name, const GLenum& shaderType, const std::string& source)
        :_name(name) {
        _shader = FF_GL_CALL(glCreateShader, shaderType);
        char const* const source_c_str = source.c_str();
        FF_GL_CALL(glShaderSource, _shader,
            1,
            &source_c_str,
            0); // 0 = Null terminated
        FF_CONSOLE_LOG("Compiling `%s`...", getName());
        FF_GL_CALL(glCompileShader, _shader);

        GLint result = GL_FALSE;
        FF_GL_CALL(glGetShaderiv, _shader, GL_COMPILE_STATUS, &result);
        if(result == GL_FALSE) {
            GLint logLength;
            FF_GL_CALL(glGetShaderiv, _shader, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<char> errorMessage(logLength + 1); // + 1 = terminating null character
            FF_GL_CALL(glGetShaderInfoLog, _shader,
                logLength,
                0, // Null-terminated string
                &errorMessage[0]);
            FF_GL_CALL(glDeleteShader, _shader);
            FF_ASSERT(false, "Shader compilation error: %s.", &errorMessage[0]);
        }

        graphicsDevice->glObjectLabel(GL_SHADER,
            _shader,
            name.size(),
            name.c_str());
    }
    GLShaderFunction::~GLShaderFunction() {
        FF_GL_CALL(glDeleteShader, _shader);
    }

    void GLShaderFunction::attachToShaderProgram(const GLuint& program) {
        FF_GL_CALL(glAttachShader, program, _shader);
    }
    void GLShaderFunction::detatchFromShaderProgram(const GLuint& program) {
        FF_GL_CALL(glDetachShader, program, _shader);
    }

    const std::string& GLShaderFunction::getName() const {
        return _name;
    }
}
