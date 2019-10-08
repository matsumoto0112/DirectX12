#include "../Utility/ShaderDefine.hlsli"

struct VSInput
{
    float4 pos : POSITION;
    float3 normal : NORMAL;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};

PSInput main(VSInput input)
{
    PSInput o = (PSInput) 0;
    o.pos = mul(input.pos, MVPMatrix());
    o.normal = mul(input.normal, (float3x3) mvp.world);
    return o;
}