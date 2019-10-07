#include "../Utility/ShaderDefine.hlsli"

struct PSInput
{
    float4 pos : SV_POSITION;
};

float4 main(const PSInput input) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}