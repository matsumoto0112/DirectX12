#include "Utility/ShaderDefine.hlsli"

Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR0;
};

float4 main(const PSInput input) : SV_Target
{
    float4 o = input.color * color;
    return o;
}