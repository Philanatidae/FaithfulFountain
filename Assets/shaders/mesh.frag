/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#version 450

layout(location=0) out vec4 FragColor;

layout(location=0) in vec4 io_color;
layout(location=1) in vec2 io_texCoord;
//layout(location=2) in vec4 i_faceNormal;

layout(binding=0) uniform sampler2D albedo;

void main() {
     FragColor = texture(albedo, io_texCoord) * io_color;
     // FragColor = vec4(i_color.rgb * i_color.a, i_color.a);
}
