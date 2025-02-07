/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <metal_stdlib>
using namespace metal;

vertex float4 v_simple(constant float4* in [[buffer(0)]], uint vid [[vertex_id]]) {
    return in[vid];
}

fragment float4 f_simple(float4 in [[stage_in]]) {
    return float4(1, 0, 0, 1);
}

struct SpriteVertexIn {
    packed_float4 position;
    packed_float2 uv;
    packed_float4 color;
};
struct SpriteVertexOut {
    float4 position [[position]];
    float2 uv;
    float4 color;
};

struct SpriteUniforms {
    float4x4 cameraMatrix;
};

vertex SpriteVertexOut vSpriteNoUniforms(constant SpriteVertexIn* vertices [[buffer(0)]],
    uint vid [[vertex_id]]) {
    SpriteVertexIn in = vertices[vid];
    SpriteVertexOut out;
    out.position = in.position;
    out.color = in.color;
    out.uv = in.uv;
    return out;
}
vertex SpriteVertexOut vSprite(constant SpriteVertexIn* vertices [[buffer(0)]],
    uint vid [[vertex_id]],
    constant SpriteUniforms& uniforms [[buffer(1)]]) {
    SpriteVertexIn in = vertices[vid];
    SpriteVertexOut out;
    float4 positionUnpacked = in.position;
    out.position = uniforms.cameraMatrix * positionUnpacked;
    out.color = in.color;
    out.uv = in.uv;
    return out;
}
fragment half4 fSprite(SpriteVertexOut in [[stage_in]],
    texture2d<float> tex2D [[texture(0)]],
    sampler sampler2D [[sampler(0)]]) {
    half4 c = half4(tex2D.sample(sampler2D, float2(in.uv.x, in.uv.y)) * in.color);
    return c;
}

float2 safeNormalize(float2 v) {
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

fragment half4 fSpriteMSDF(SpriteVertexOut in [[stage_in]],
    texture2d<float> tex2D [[texture(0)]],
    sampler sampler2D [[sampler(0)]]) {
    // Convert normalized texture cooridnates to absolute texture coordinates
    float2 uvScreen = in.uv * float2(tex2D.get_width(), tex2D.get_height());

    // Calculate derivatives
    float2 Jdx = dfdx(uvScreen);
    float2 Jdy = dfdy(uvScreen);

    // Sample textures 
    float3 samp = tex2D.sample(sampler2D, float2(in.uv.x, in.uv.y)).rgb;

    // Calculate the signed distance (in texels)
    float sigDist = median(samp.r, samp.g, samp.b) - 0.5;

    // For proper anti-aliasing we need to calculate the signed distance in pixels.
    // We do this using the derivatives.
    float2 gradDist = safeNormalize(float2(dfdx(sigDist), dfdy(sigDist)));
    float2 grad = float2(gradDist.x * Jdx.x + gradDist.y * Jdy.x, gradDist.x * Jdx.y + gradDist.y * Jdy.y);

    // Apply anti-aliasing
    const float thickness = 0.125;
    const float normalization = thickness * 0.5f * sqrt(2.0f);

    float afWidth = min(normalization * length(grad), 0.5);
    float opacity = smootherstep(-afWidth, afWidth, sigDist);

    // Apply pre-multiplied alpha with gamma correction
    half4 color;
    color.a = half(pow(abs(in.color.a * opacity), 1.0f / 2.2f));
    color.rgb = half3(in.color.rgb * color.a);
    return color;
}

struct QuadVertexIn {
    packed_float4 position;
    packed_float2 uv;
    packed_float4 color;
};
struct QuadVertexOut {
    float4 position [[position]];
    float2 uv;
    float4 color;
};

struct QuadUniforms {
    float warpFactor;
};

vertex QuadVertexOut vQuad(constant QuadVertexIn* vertices [[buffer(0)]],
    uint vid [[vertex_id]]) {
    QuadVertexIn in = vertices[vid];
    QuadVertexOut out;
    out.position = in.position;
    out.color = in.color;
    out.uv = in.uv;
    return out;
}
float2 SineWave(float2 p, float period, float time, float speed, float amplitude) {
    const float pi = 3.1415;
    float w = period * 10 * pi;
    float t = time * speed * pi / 180;
    float f = sin(w * p.y + t) * amplitude;
    return float2(p.x + f, p.y);
}
fragment half4 fQuad(QuadVertexOut in [[stage_in]],
    texture2d<float> tex2D [[texture(0)]],
    sampler sampler2D [[sampler(0)]],
    constant QuadUniforms& uniforms [[buffer(1)]]) {
    float2 uv = SineWave(in.uv, 0.75f, 0, 0, uniforms.warpFactor);
    half4 c = half4(tex2D.sample(sampler2D, float2(uv.x, uv.y)) * in.color);
    return c;
}
