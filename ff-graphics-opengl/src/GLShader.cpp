/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff-graphics-opengl/GLShader.hpp>

#include <ff-graphics-opengl/GLGlobalState.hpp>

#include <ff/Console.hpp>
#include <vector>

namespace ff {
    GLShader::GLShader(GLGraphicsDevice* const& graphicsDevice,
        const std::string& name,
        GLShaderFunction& vertexFunction,
        GLShaderFunction& fragmentFunction,
        const std::vector<std::pair<std::string, int>>& textureBindings,
        const std::vector<std::pair<std::string, int>>& uniformBufferBindings)
        :_graphicsDevice(graphicsDevice),
        _name(name),
        _textureBindings(textureBindings),
        _uniformBufferBindings(uniformBufferBindings) {
        _program = FF_GL_CALL(glCreateProgram);

        vertexFunction.attachToShaderProgram(_program);
        fragmentFunction.attachToShaderProgram(_program);

        FF_CONSOLE_LOG("Linking `%s`...", getName());
        FF_GL_CALL(glLinkProgram, _program);

        vertexFunction.detatchFromShaderProgram(_program);
        fragmentFunction.detatchFromShaderProgram(_program);

        GLint result;
        FF_GL_CALL(glGetProgramiv, _program, GL_LINK_STATUS, &result);
        if(result == GL_FALSE) {
            GLint logLength;
            FF_GL_CALL(glGetProgramiv, _program, GL_INFO_LOG_LENGTH, &logLength);
            std::vector<char> errorMessage(logLength + 1); // + 1 = terminating null character
            FF_GL_CALL(glGetProgramInfoLog, _program,
                logLength,
                0, // Null-terminated string
                &errorMessage[0]);
            FF_GL_CALL(glDeleteProgram, _program);
            FF_ASSERT(false, "Program link error: %s.", &errorMessage[0]);
        }

        graphicsDevice->glObjectLabel(GL_PROGRAM,
            _program,
            name.size(),
            name.c_str());

        bind();
        applyTextureBindings();
        applyUniformBufferBindings();
    }
    GLShader::~GLShader() {
        FF_GL_CALL(glDeleteProgram, _program);
    }

    void GLShader::bind() {
        _graphicsDevice->getGlobalState()->bindProgram(_program);
    }

    const std::string& GLShader::getName() const {
        return _name;
    }

    void GLShader::applyTextureBindings() {
        for(auto& bindingPair : _textureBindings) {
            GLuint uniform = FF_GL_CALL(glGetUniformLocation, _program, bindingPair.first.c_str());
            FF_GL_CALL(glUniform1i, uniform, bindingPair.second);
        }
    }
    void GLShader::applyUniformBufferBindings() {
        for(auto& bindingPair : _uniformBufferBindings) {
            GLuint uniform = FF_GL_CALL(glGetUniformBlockIndex, _program, bindingPair.first.c_str());
            FF_GL_CALL(glUniformBlockBinding, _program, uniform, bindingPair.second);
        }
    }
}
