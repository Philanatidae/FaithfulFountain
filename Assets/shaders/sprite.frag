/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#version 450

layout(location=0) in vec2 io_uv;
layout(location=1) in vec4 io_color;

layout(location=0) out vec4 FragColor;

layout(binding=0) uniform sampler2D image;

void main() {
    FragColor = texture(image, io_uv) * io_color;
}
