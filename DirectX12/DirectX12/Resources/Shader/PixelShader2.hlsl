cbuffer Color : register(b0)
{
    float4 color;
}

Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

struct PSInput
{
    float4 pos : SV_POSITION;
};

float4 main(const PSInput input) : SV_Target
{
    float4 o = 1.0f - color;
    return o;
}