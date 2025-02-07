/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FF_IO_GLMSERIALIZERS_HPP
#define _FF_IO_GLMSERIALIZERS_HPP

#include <glm/glm.hpp>
#include <ff/io/Serializer.hpp>

namespace ff {

template<>
class CustomSerializer<glm::vec2> {
    static void serialize(Serializer* serializer,
        char const* const label,
        glm::vec2& x);
};
template<>
class CustomSerializer<glm::vec3> {
    static void serialize(Serializer* serializer,
        char const* const label,
        glm::vec3& x);
};
template<>
class CustomSerializer<glm::vec4> {
    static void serialize(Serializer* serializer,
        char const* const label,
        glm::vec4& x);
};

}

#endif

