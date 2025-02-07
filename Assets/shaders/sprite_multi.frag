/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#version 450

#define FF_SPRITE_MULTI_TEXTURE_COUNT 8

layout(location=0) in vec2 io_uv;
layout(location=1) in vec4 io_color;
layout(location=2) in flat int io_texI;

layout(location=0) out vec4 FragColor;

layout(binding=0) uniform sampler2D u_textures[FF_SPRITE_MULTI_TEXTURE_COUNT];

vec4 SampleTextureColor() {
#if defined(ES)
    switch(io_texI) {
    case 0:
        return texture(u_textures[0], io_uv);
    case 1:
        return texture(u_textures[1], io_uv);
    case 2:
        return texture(u_textures[2], io_uv);
    case 3:
        return texture(u_textures[3], io_uv);
    case 4:
        return texture(u_textures[4], io_uv);
    case 5:
        return texture(u_textures[5], io_uv);
    case 6:
        return texture(u_textures[6], io_uv);
    case 7:
        return texture(u_textures[7], io_uv);

    default:
        return vec4(1, 0, 1, 1); // We don't have an assert so just have a bright color that will be obvious
    }
#else
    return texture(u_textures[io_texI], io_uv);
#endif
}

void main() {
    FragColor = SampleTextureColor() * io_color;
}
