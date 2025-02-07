/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_SHADER_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_SHADER_HPP

#include <string>

namespace ff {    
    class IShader {
    public:
        virtual ~IShader() = default;
    };

    class NullShader : public IShader {
    };
}

#endif
