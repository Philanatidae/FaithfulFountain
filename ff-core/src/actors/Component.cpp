/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/actors/Component.hpp>

#include <unordered_map>

#include <ff/Console.hpp>

namespace ff {
    namespace _internal {
        ComponentType getNextComponentType() {
            static ComponentType nextType = 1;
            FF_ASSERT(nextType != 0, "Max components reached (can only register up to %i).", FF_MAX_COMPONENTS);
            ComponentType type = nextType;
            nextType <<= 1;
            return type;
        }
        ComponentType getComponentType(const std::type_index& type) {
            static std::unordered_map<std::type_index, ComponentType> typeMap;
            if (typeMap.find(type) == typeMap.end()) {
                typeMap.emplace(type, getNextComponentType());
            }
            return typeMap[type];
        }
    }
}
