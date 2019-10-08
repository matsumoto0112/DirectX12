#include "../Utility/ShaderDefine.hlsli"

cbuffer Light : register(b3)
{
    float4x4 lightVP;
}

struct VSInput
{
    float4 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 posSM : SM_POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

PSInput main(VSInput input)
{
    PSInput o = (PSInput) 0;
    o.pos = mul(input.pos, MVPMatrix());
    o.normal = mul(float4(input.normal, 0.0f), mvp.world);
    o.uv = input.uv;
    float4 pos = mul(input.pos, mvp.world);
    pos = mul(pos, lightVP);
    pos.xyz /= pos.w;
    o.posSM.x = (pos.x + 1.0f) / 2.0f;
    o.posSM.y = (1.0f - pos.y) / 2.0f;
    o.posSM.z = pos.z;

    return o;
}