cbuffer MAPMatrix : register(b1)
{
    matrix world;
    matrix view;
    matrix proj;
}

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
    matrix m = mul(world, view);
    m = mul(m, proj);

    o.pos = mul(input.pos, m);
    return o;
}