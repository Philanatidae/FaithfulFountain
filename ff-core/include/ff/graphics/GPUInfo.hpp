/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_GPU_MEMORY_TYPE_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_GPU_MEMORY_TYPE_HPP

#include <string>

namespace ff {
    enum class GPUMemoryType {
        DISCRETE,
        SHARED,
        UNKNOWN
    };
    std::string gpuMemoryTypeToString(const GPUMemoryType& type);

    enum class GPUType {
        INTERMEDIATE,
        TILE,
        UNKNOWN
    };
    std::string gpuTypeToString(const GPUType& type);

    enum class UVCoordSign {
        POSITIVE_Y_UP,
        NEGATIVE_Y_UP,
    };
    std::string uvCoordSignToString(const UVCoordSign& sign);

    enum class ClipSpaceHandedness {
        LEFT,
        RIGHT,
        UNKNOWN
    };
    std::string clipSpaceHandednessToString(ClipSpaceHandedness const& hand);
}

#endif
