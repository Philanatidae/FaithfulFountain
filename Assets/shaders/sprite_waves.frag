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

// @todo Would love it if I could combine sprite.frag and sprite_msdf.frag into a single file with multiple functions. Maybe it's possible? Or use .h includes?

layout(binding=1, std140) uniform WaveUniforms {
    float waveFactor;
};

vec2 HorizontalSinusoidal(vec2 p, float period, float time, float speed, float amplitude) {
    const float pi = 3.1415;
    float w = period * 10 * pi;
    float t = time * speed * pi / 180;
    float f = sin(w * p.y + t) * amplitude;
    return vec2(p.x + f, p.y);
}

void main() {
    vec2 modUV = HorizontalSinusoidal(io_uv, 0.75, 0, 0, waveFactor);
    FragColor = texture(image, modUV) * io_color;
}
