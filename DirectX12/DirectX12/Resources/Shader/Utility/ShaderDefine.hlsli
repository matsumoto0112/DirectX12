cbuffer MVP : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
};

cbuffer Color : register(b1)
{
    float4 color;
}