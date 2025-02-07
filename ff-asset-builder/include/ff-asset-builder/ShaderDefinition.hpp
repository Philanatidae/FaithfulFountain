/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_ASSET_BUILDER_SHADER_DEFINITION_HPP
#define _FAITHFUL_FOUNTAIN_ASSET_BUILDER_SHADER_DEFINITION_HPP

#include <string>

namespace ff {
    struct ShaderDefinition {
        std::string name;
        std::string vertexFunction;
        std::string fragmentFunction;

        ShaderDefinition(const std::string& name,
            const std::string& vertexFunction,
            const std::string& fragmentFunction) :
            name(name),
            vertexFunction(vertexFunction),
            fragmentFunction(fragmentFunction) {
        }
    };
}

#endif
