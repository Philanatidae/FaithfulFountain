/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef _FAITHFUL_FOUNTAIN_GRAPHICS_TEXTURE_TYPES_HPP
#define _FAITHFUL_FOUNTAIN_GRAPHICS_TEXTURE_TYPES_HPP

#include <stdint.h>

namespace ff {

using TextureFlag_t = uint8_t;
namespace TextureFlags {
}

using TextureUsage_t = uint8_t;
namespace TextureUsage {
    constexpr TextureFlag_t GPU_RENDER_TARGET = 1 << 0; // Can be used as a render target

    constexpr TextureFlag_t GPU_STORE = 1 << 1; // Can be stored at the end of a render pass
    constexpr TextureFlag_t GPU_LOAD = 1 << 2; // Can be loaded at the start of a render pass (RenderTargetLoadAction::LOAD can be used)

    constexpr TextureFlag_t GPU_SAMPLE = 1 << 3; // Can be sampled by a shader
    
    constexpr TextureFlag_t GPU_BLIT = 1 << 4; // Blit operations can be performed
        
    constexpr TextureFlag_t CPU_READ = 1 << 5;
    constexpr TextureFlag_t CPU_WRITE = 1 << 6;
}

enum class TextureType {
    COLOR,
    DEPTH,
    STENCIL,
    DEPTH_STENCIL
};

}

#endif
