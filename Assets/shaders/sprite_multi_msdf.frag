/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#version 450

layout(location=0) in vec2 io_uv;
layout(location=1) in vec4 io_color;
layout(location=2) in flat int io_texI;

layout(location=0) out vec4 FragColor;

layout(binding=0) uniform sampler2D u_textures[8];

// @todo Would love it if I could combine sprite.frag and sprite_msdf.frag into a single file with multiple functions. Maybe it's possible? Or use .h includes?

vec2 safeNormalize(vec2 v) {
    float len = length(v);
    len = (len > 0.0) ? 1.0 / len : 0.0;
    return v * len;
}

float median(float a, float b, float c) {
    return max(min(a, b), min(max(a, b), c));
}

float smootherstep(float e1, float e2, float x) {
    x = clamp((x - e1) / (e2 - e1), 0.0, 1.0);
    return x * x * x * (x * (x * 6 - 15) + 10);
}

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

vec2 SampleTextureSize() {
#if defined(ES)
    switch(io_texI) {
    case 0:
        return textureSize(u_textures[0], 0);
    case 1:
        return textureSize(u_textures[1], 0);
    case 2:
        return textureSize(u_textures[2], 0);
    case 3:
        return textureSize(u_textures[3], 0);
    case 4:
        return textureSize(u_textures[4], 0);
    case 5:
        return textureSize(u_textures[5], 0);
    case 6:
        return textureSize(u_textures[6], 0);
    case 7:
        return textureSize(u_textures[7], 0);

    default:
        return vec2(0, 0); // We don't have an assert so just return (0, 0)
    }
#else
    return textureSize(u_textures[io_texI], 0);
#endif
}

void main() {
    // Convert normalized texture cooridnates to absolute texture coordinates
    vec2 uvScreen = io_uv * SampleTextureSize();

    // Calculate derivatives
    vec2 Jdx = dFdx(uvScreen);
    vec2 Jdy = dFdy(uvScreen);

    // Sample textures 
    vec3 samp = SampleTextureColor().rgb;

    // Calculate the signed distance (in texels)
    float sigDist = median(samp.r, samp.g, samp.b) - 0.5;

    // For proper anti-aliasing we need to calculate the signed distance in pixels.
    // We do this using the derivatives.
    vec2 gradDist = safeNormalize(vec2(dFdx(sigDist), dFdy(sigDist)));
    vec2 grad = vec2(gradDist.x * Jdx.x + gradDist.y * Jdy.x, gradDist.x * Jdx.y + gradDist.y * Jdy.y);

    // Apply anti-aliasing
    const float thickness = 0.125;
    const float normalization = thickness * 0.5f * sqrt(2.0f);

    float afWidth = min(normalization * length(grad), 0.5);
    float opacity = smootherstep(-afWidth, afWidth, sigDist);

    // // Apply pre-multiplied alpha with gamma correction
    // vec4 color;
    // color.a = float(pow(abs(io_color.a * opacity), 1.0f / 2.2f));
    // color.rgb = vec3(io_color.rgb * color.a);
    
    // Apply pre-multiplied alpha
    vec4 color;
    color.a = io_color.a * opacity;
    color.rgb = vec3(io_color.rgb * color.a);

    FragColor = color;
}
