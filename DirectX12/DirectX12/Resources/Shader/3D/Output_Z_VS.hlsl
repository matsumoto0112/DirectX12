#include "../Utility/ShaderDefine.hlsli"

cbuffer Light : register(b3)
{
    float4x4 lightVP;
};

struct VSInput
{
    float4 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

float4 main(VSInput input) : SV_POSITION
{
    float4 pos = mul(input.pos, mvp.world);
    pos = mul(pos, lightVP);
    return pos;
}