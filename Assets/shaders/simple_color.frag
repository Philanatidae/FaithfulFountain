/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#version 450

layout(location=0) out vec4 o_color;

layout(location=0) in vec4 io_color;

void main() {
    // Pre-multiplied alpha
    o_color = vec4(io_color.rgb * io_color.a, io_color.a);
}
