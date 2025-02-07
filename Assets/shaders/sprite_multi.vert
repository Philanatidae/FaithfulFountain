/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#version 450

layout(location=0) in vec4 a_position;
layout(location=1) in vec2 a_uv;
layout(location=3) in int a_texI;
layout(location=2) in vec4 a_color;

layout(binding=0, std140) uniform Uniforms {
   mat4 camera;
};

layout(location=0) out vec2 io_uv;
layout(location=1) out vec4 io_color;
layout(location=2) flat out int io_texI;

void main() {
    gl_Position = camera * a_position;

    io_uv = a_uv;
    io_color = a_color;
    io_texI = a_texI;
}
