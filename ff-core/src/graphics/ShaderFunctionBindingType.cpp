/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/ShaderFunctionBindingType.hpp>

#include <ff/Console.hpp>

namespace ff {
    std::string convertShaderFunctionBindingTypeToString(const ShaderFunctionBindingType& type) {
        switch(type) {
        case ShaderFunctionBindingType::Texture:
            return "texture";
        case ShaderFunctionBindingType::UniformBuffer:
            return "uniform_buffer";
        default:
            FF_ASSERT(false, "Unknown shader function binding type.");
        }
    }
    ShaderFunctionBindingType convertStringToShaderFunctionBindingType(const std::string& type) {
        if(type == "texture") {
            return ShaderFunctionBindingType::Texture;
        }
        if(type == "uniform_buffer") {
            return ShaderFunctionBindingType::UniformBuffer;
        }
        FF_ASSERT(false, "Unknown conversion of %s to shader function binding type.", type);
    }
}
