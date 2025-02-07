/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <ff/graphics/GPUInfo.hpp>

#include <ff/Console.hpp>

namespace ff {
    std::string gpuMemoryTypeToString(const GPUMemoryType& type) {
        switch (type) {
        case GPUMemoryType::DISCRETE:
            return "DISCRETE";
        case GPUMemoryType::SHARED:
            return "SHARED";
        case GPUMemoryType::UNKNOWN:
        default:
            return "UNKNOWN";
        }
    }

    std::string gpuTypeToString(const GPUType& type) {
        switch (type) {
        case GPUType::INTERMEDIATE:
            return "INTERMEDIATE";
        case GPUType::TILE:
            return "TILE";
        case GPUType::UNKNOWN:
        default:
            return "UNKNOWN";
        }
    }

    std::string uvCoordSignToString(const UVCoordSign& sign) {
        switch (sign) {
        case UVCoordSign::POSITIVE_Y_UP:
            return "POSITIVE_Y_UP";
        case UVCoordSign::NEGATIVE_Y_UP:
            return "NEGATIVE_Y_UP";
        default:
            FF_CONSOLE_FATAL("UVCoordSign must be known.");
            return "UNKNOWN";
        }
    }

    std::string clipSpaceHandednessToString(ClipSpaceHandedness const& hand) {
        switch(hand) {
        case ClipSpaceHandedness::LEFT:
            return "LEFT";
        case ClipSpaceHandedness::RIGHT:
            return "RIGHT";
        default:
            FF_CONSOLE_FATAL("Handedness must be known.");
            return "UNKNOWN";
        }
    }
}
