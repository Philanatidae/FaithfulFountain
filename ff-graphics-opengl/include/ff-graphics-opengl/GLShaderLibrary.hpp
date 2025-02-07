/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_SHADER_LIBRARY_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_OPENGL_GL_SHADER_LIBRARY_HPP

#include <unordered_map>
#include <string>
#include <ff-graphics-opengl/GLShader.hpp>
#include <ff-graphics-opengl/GLShaderFunction.hpp>
#include <ff-graphics-opengl/GLGraphicsDevice.hpp>
#include <ff/assets/IAssetBundle.hpp>

namespace ff {
    class GLShaderLibrary final {
    public:
        GLShaderLibrary(ff::IAssetBundle& assetBundle, const nlohmann::json& assetObject);
        ~GLShaderLibrary();

        std::shared_ptr<GLShader> getShader(const std::string& name) const;
    private:
        GLGraphicsDevice* const _graphicsDevice;
        std::unordered_map<std::string, std::shared_ptr<GLShader>> _shaders;
        
        GLShaderFunction& getOrCreateFunctionFromCache(const std::string& function, const GLenum& shaderType, IAssetBundle& assetBundle, std::unordered_map<std::string, std::unique_ptr<GLShaderFunction>>& cache);
    };
}

#endif
