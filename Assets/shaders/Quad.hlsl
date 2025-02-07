/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

struct QuadVertexIn {
    float4 position: POSITION;
    float2 uv: TEXCOORD;
    float4 color: COLOR;
};
struct QuadVertexOut {
    float4 position: SV_POSITION;
    float2 uv: TEXCOORD;
    float4 color: COLOR;
};

cbuffer QuadUniformsBuffer : register(b0)
{
    float warpFactor;
};

Texture2D quadTexture : register(t0);
SamplerState quadSampler : register(s0);

QuadVertexOut vQuad(QuadVertexIn input) {
    QuadVertexOut output = (QuadVertexOut)0;

    output.position = input.position;
    output.uv = input.uv;
    output.color = input.color;

    return output;
}
float2 SineWave(float2 p, float period, float time, float speed, float amplitude) {
    float pi = 3.1415;
    float w = period * 10 * pi;
    float t = time * speed * pi / 180;
    float f = sin(w * p.y + t) * amplitude;
    return float2(p.x + f, p.y);
}
float4 fQuad(QuadVertexOut input) : SV_Target{
    float2 uv = SineWave(input.uv, 0.75f, 0, 0, warpFactor);
    float4 color = quadTexture.Sample(quadSampler, uv) * input.color;
    return color;
}
