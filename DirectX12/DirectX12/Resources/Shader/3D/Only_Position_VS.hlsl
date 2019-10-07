#include "../Utility/ShaderDefine.hlsli"

struct VSInput
{
    float4 pos : POSITION;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
};

VSOutput main(const VSInput input)
{
    VSOutput o = (VSOutput) 0;
    o.pos = mul(input.pos, MVPMatrix());
    return o;
}