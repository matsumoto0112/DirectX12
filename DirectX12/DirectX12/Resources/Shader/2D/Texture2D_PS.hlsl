Texture2D tex : register(t0);
SamplerState samLinear : register(s0);

#include "../Utility/ShaderDefine.hlsli"

struct ps_input
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(ps_input input) : SV_TARGET
{
    float4 o = tex.Sample(samLinear, input.uv);
    o *= color;
    clip(o.a - EPSILON);
    return o;
}