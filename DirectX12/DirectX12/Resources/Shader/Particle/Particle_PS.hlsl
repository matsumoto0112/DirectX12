struct PSInput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
};

float4 PSMain(const PSInput input) : SV_TARGET
{
    return input.col;
}
