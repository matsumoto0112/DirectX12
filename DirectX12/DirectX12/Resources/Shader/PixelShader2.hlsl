#include "Utility/ShaderDefine.hlsli"

Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

float4 main(const PSInput input) : SV_Target
{
    float4 o = tex.Sample(samplerState, input.uv);
    o = o * input.color;
    o = o * color;
    o.rgb = 1.0f - o.rgb;
    clip(o.a - EPSILON);
    return o;
}