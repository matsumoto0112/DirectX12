#include "../Utility/ShaderDefine.hlsli"

struct VSInput
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOutput main(VSInput input)
{
    VSOutput o = (VSOutput) 0;
    o.position = mul(o.position, MVPMatrix());
    o.uv = input.uv * float2(uv.width, uv.height) + float2(uv.left, uv.top);
    return o;
}