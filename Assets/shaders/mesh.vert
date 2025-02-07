/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#version 450

layout(location=0) in vec4 a_position;
layout(location=1) in vec4 a_color;
layout(location=2) in vec2 a_texCoord;
//layout(location=3) in vec4 a_normal;

layout(binding=0, std140) uniform CameraUniforms {
    mat4 camera;
};
layout(binding=1, std140) uniform MeshUniforms {
    mat4 worldMat;
};
layout(binding=2, std140) uniform MaterialUniforms {
    vec4 matTintColor;
};

layout(location=0) out vec4 io_color;
layout(location=1) out vec2 io_texCoord;
//layout(location=2) out vec4 o_faceNormal;

void main() {
    gl_Position = camera * worldMat * a_position;

    io_color = matTintColor * a_color;
    io_texCoord = a_texCoord;
    //o_faceNormal = a_normal;
}
