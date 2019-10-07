#include "../Utility/ShaderDefine.hlsli"

struct VSInput
{
    float4 pos : POSITION0;
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOutput main(const VSInput input)
{
    VSOutput o = (VSOutput) 0;
    o.pos = mul(input.pos, MVPMatrix());
    o.uv = input.uv;
    return o;
}