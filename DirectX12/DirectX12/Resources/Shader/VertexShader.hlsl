#include "Utility/ShaderDefine.hlsli"

struct VSInput
{
    float4 pos : POSITION0;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
};

VSOutput main(const VSInput input)
{
    VSOutput o = (VSOutput) 0;
    matrix m = mul(world, view);
    m = mul(m, proj);

    o.pos = mul(input.pos, m);
    o.uv = input.uv;
    o.color = input.color;
    return o;
}