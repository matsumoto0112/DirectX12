static const float EPSILON = 1e-6; //�덷

//���[���h�E�r���[�E�v���W�F�N�V�����s��
struct WorldViewProjection
{
    matrix world;
    matrix view;
    matrix proj;
};

//UV���
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