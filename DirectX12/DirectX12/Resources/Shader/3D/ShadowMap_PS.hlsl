Texture2D tex : register(t0);
Texture2D<float> shadow : register(t1);
SamplerState samplerState : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 posSM : SM_POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

float4 main(PSInput input) : SV_TARGET
{
    float sm = shadow.Sample(samplerState, input.posSM.xy);
    float sma = (input.posSM.z - 0.005f < sm) ? 1.0f : 0.5f;

    return float4(1.0f, 0.0f, 0.0f, 1.0f) * sma;
}