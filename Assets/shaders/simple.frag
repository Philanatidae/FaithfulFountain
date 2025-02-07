/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#version 450

layout(location=0) out vec4 color;

void main() {
    color = vec4(1.0, 0.0, 0.0, 1.0);
    color.r = 1;
    // Pre-multiplied alpha
    color.rgb = color.rgb * color.a;
}
