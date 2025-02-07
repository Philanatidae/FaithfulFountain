/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_SHADER_FUNCTION_BINDING_TYPE_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_SHADER_FUNCTION_BINDING_TYPE_HPP

#include <string>

namespace ff {
    enum class ShaderFunctionBindingType {
        Texture,
        UniformBuffer
    };
    std::string convertShaderFunctionBindingTypeToString(const ShaderFunctionBindingType& type);
    ShaderFunctionBindingType convertStringToShaderFunctionBindingType(const std::string& type);
}

#endif
