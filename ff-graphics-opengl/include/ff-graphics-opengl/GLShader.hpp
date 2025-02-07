/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_SHADER_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_SHADER_HPP

#include <ff/graphics/IShader.hpp>
#include <ff-graphics-opengl/gl.hpp>
#include <ff-graphics-opengl/GLGraphicsDevice.hpp>
#include <ff-graphics-opengl/GLShaderFunction.hpp>
#include <string>
#include <utility>
#include <vector>

namespace ff {
    class GLShader final : public IShader {
    public:
        GLShader(GLGraphicsDevice* const& graphicsDevice,
            const std::string& name,
            GLShaderFunction& vertexFunction,
            GLShaderFunction& fragmentFunction,
            const std::vector<std::pair<std::string, int>>& textureBindings,
            const std::vector<std::pair<std::string, int>>& uniformBufferBindings);
        ~GLShader();

        void bind();

        const std::string& getName() const;
    private:
        GLGraphicsDevice* const _graphicsDevice;
        std::string const _name;
        GLuint _program;
        std::vector<std::pair<std::string, int>> _textureBindings;
        std::vector<std::pair<std::string, int>> _uniformBufferBindings;

        void applyTextureBindings();
        void applyUniformBufferBindings();
    };
}

#endif
