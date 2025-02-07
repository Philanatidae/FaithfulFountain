/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/io/GLMSerializers.hpp>

namespace ff {


void CustomSerializer<glm::vec2>::serialize(Serializer* serializer,
    char const* const label,
    glm::vec2& x) {
    serializer->beginObject(label);
    serializer->serialize("x", x.x);
    serializer->serialize("y", x.y);
    serializer->endObject();
}
void CustomSerializer<glm::vec3>::serialize(Serializer* serializer,
    char const* const label,
    glm::vec3& x) {
    serializer->beginObject(label);
    serializer->serialize("x", x.x);
    serializer->serialize("y", x.y);
    serializer->serialize("z", x.z);
    serializer->endObject();
}
void CustomSerializer<glm::vec4>::serialize(Serializer* serializer,
    char const* const label,
    glm::vec4& x) {
    serializer->beginObject(label);
    serializer->serialize("x", x.x);
    serializer->serialize("y", x.y);
    serializer->serialize("z", x.z);
    serializer->serialize("w", x.w);
    serializer->endObject();
}

}
