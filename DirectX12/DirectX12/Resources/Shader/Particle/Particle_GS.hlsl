static const float EPSILON = 1e-6; //誤差

//ワールド・ビュー・プロジェクション行列
struct WorldViewProjection
{
    matrix world;
    matrix view;
    matrix proj;
};

cbuffer CB0 : register(b0)
{
    WorldViewProjection mvp;
};

matrix MVPMatrix()
{
    matrix m = mul(mvp.world, mvp.view);
    m = mul(m, mvp.proj);
    return m;
}
struct GSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
};

#define MAX_VERTEX_COUNT (6)
[maxvertexcount(MAX_VERTEX_COUNT)]
void GSMain(point GSInput input[1], inout TriangleStream<PSInput> output)
{
    float4x4 m = MVPMatrix();

    float4 pos[4] =
    {
        float4(-0.5f, 0.5f, 0.0f, 1.0f),
        float4(0.5f, 0.5f, 0.0f, 1.0f),
        float4(0.5f, -0.5f, 0.0f, 1.0f),
        float4(-0.5f, -0.5f, 0.0f, 1.0f),
    };

    uint indices[MAX_VERTEX_COUNT] =
    {
        0, 1, 2, 0, 2, 3,
    };

    PSInput o = (PSInput) 0;
    o.col = input[0].color;

    uint i;
    [unroll]
    for (i = 1; i <= MAX_VERTEX_COUNT; i++)
    {
        o.pos = pos[indices[i - 1]];
        o.pos += input[0].position;
        o.pos = mul(o.pos, m);
        output.Append(o);
        if (i % 3 == 0)
            output.RestartStrip();
    }
}
