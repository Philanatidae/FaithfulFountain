/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#version 450

layout(location=0) in vec4 a_position;

layout(binding=0, std140) uniform CameraUniforms {
    mat4 camera;
};
layout(binding=1, std140) uniform MeshUniforms {
    mat4 worldMat;
};
layout(binding=2, std140) uniform ActorUniforms {
    uvec2 actorID;
};

layout(location=0) out flat uvec2 io_actorID;

void main() {
    gl_Position = camera * worldMat * a_position;
    io_actorID = actorID;
}

