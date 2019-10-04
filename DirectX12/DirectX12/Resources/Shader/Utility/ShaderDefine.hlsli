static const float EPSILON = 1e-6; //誤差

//ワールド・ビュー・プロジェクション行列
struct WorldViewProjection
{
    matrix world;
    matrix view;
    matrix proj;
};

//UV情報
struct UVInfo
{
    float left;
    float top;
    float width;
    float height;
};


cbuffer CB0 : register(b0)
{
    WorldViewProjection mvp;
};

cbuffer CB1 : register(b1)
{
    float4 color;
}

cbuffer CB2 : register(b2)
{
    UVInfo uv;
}

matrix MVPMatrix()
{
    matrix m = mul(mvp.world, mvp.view);
    m = mul(m, mvp.proj);
    return m;
}