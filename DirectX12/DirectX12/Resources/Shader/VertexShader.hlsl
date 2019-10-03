cbuffer MAPMatrix : register(b1) {
    matrix world;
    matrix view;
    matrix proj;
}

struct VSInput {
    float4 pos : POSITION;
    float2 uv : TEXCOORD0;
};

struct VSOutput {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOutput main(const VSInput input) {
    VSOutput o = (VSOutput)0;
    matrix m = mul(world, view);
    m = mul(m, proj);

    o.pos = mul(input.pos, m);
    //o.pos = input.pos;
    o.uv = input.uv;
    return o;
}