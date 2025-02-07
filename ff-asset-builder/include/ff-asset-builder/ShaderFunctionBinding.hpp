/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ASSET_BUILDER_SHADER_FUNCTION_BINDING_HPP
#define _FAITHFUL_FOUNTAIN_ASSET_BUILDER_SHADER_FUNCTION_BINDING_HPP

#include <string>
#include <ff/graphics/ShaderFunctionBindingType.hpp>

namespace ff {
    struct ShaderFunctionBinding {
        std::string name;
        int binding;
        ShaderFunctionBindingType type;

        ShaderFunctionBinding(const std::string& name,
            const int& binding,
            const ShaderFunctionBindingType& type)
            :name(name),binding(binding),type(type) {
        }
    };
}

#endif
