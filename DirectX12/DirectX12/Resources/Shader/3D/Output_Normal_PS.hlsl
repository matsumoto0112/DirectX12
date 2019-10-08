struct PSInput
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};

float4 main(PSInput input) : SV_TARGET
{
    float3 col = (input.normal + 1) * 0.5f;
    return float4(0.0f, 0.0f, col.b, 1.0f);
}