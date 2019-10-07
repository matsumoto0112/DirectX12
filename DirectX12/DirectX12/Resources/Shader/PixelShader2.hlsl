#include "Utility/ShaderDefine.hlsli"
#include "Utility/UtilFunc.hlsli"

Texture2D tex : register(t0);
Texture2D tex2 : register(t1);
SamplerState samplerState : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR0;
};

float4 main(PSInput input) : SV_Target
{
    //float4 t1 = tex.Sample(samplerState, input.uv);
    //float4 t2 = tex2.Sample(samplerState, input.uv);
    //return lerp(t1, t2, input.uv.x);
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
    //return tex.Sample(samplerState, input.uv);

}