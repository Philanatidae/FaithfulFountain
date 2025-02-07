/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

struct SpriteVertexIn {
    float4 position: POSITION;
    float2 uv: TEXCOORD;
    float4 color: COLOR;
};
struct SpriteVertexOut {
    float4 position: SV_POSITION;
    float2 uv: TEXCOORD;
    float4 color: COLOR;
};

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    float4x4 cameraMatrix;
};

Texture2D spriteTexture : register(t0);
SamplerState spriteSampler : register(s0);

SpriteVertexOut vSprite(SpriteVertexIn input) {
    SpriteVertexOut output = (SpriteVertexOut)0;

    output.position = mul(cameraMatrix, input.position);
    output.uv = input.uv;
    output.color = input.color;

    return output;
}
float4 fSprite(SpriteVertexOut input) : SV_Target {
    float4 color = spriteTexture.Sample(spriteSampler, input.uv) * input.color;
    return color;
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

float4 fMSDF(SpriteVertexOut input) : SV_Target {
    // Convert normalized texture cooridnates to absolute texture coordinates
    uint texWidth;
    uint texHeight;
    spriteTexture.GetDimensions(texWidth, texHeight);
    float2 uvScreen = input.uv * float2(texWidth, texHeight);

    // Calculate derivatives
    float2 Jdx = ddx(uvScreen);
    float2 Jdy = ddy(uvScreen);

    // Sample textures 
    float3 samp = spriteTexture.Sample(spriteSampler, input.uv).rgb;

    // Calculate the signed distance (in texels)
    float sigDist = median(samp.r, samp.g, samp.b) - 0.5;

    // For proper anti-aliasing we need to calculate the signed distance in pixels.
    // We do this using the derivatives.
    float2 gradDist = safeNormalize(float2(ddx(sigDist), ddy(sigDist)));
    float2 grad = float2(gradDist.x * Jdx.x + gradDist.y * Jdy.x, gradDist.x * Jdx.y + gradDist.y * Jdy.y);

    // Apply anti-aliasing
    const float thickness = 0.125;
    const float normalization = thickness * 0.5f * sqrt(2.0f);

    float afWidth = min(normalization * length(grad), 0.5);
    float opacity = smootherstep(-afWidth, afWidth, sigDist);

    // Apply pre-multiplied alpha with gamma correction
    float4 color;
    color.a = float(pow(abs(input.color.a * opacity), 1.0f / 2.2f));
    color.rgb = float3(mul(input.color.rgb, color.a));
    return color;
}
