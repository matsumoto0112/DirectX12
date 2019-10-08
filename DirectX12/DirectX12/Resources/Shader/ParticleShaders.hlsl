//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
#include "Utility/ShaderDefine.hlsli"

struct VSInput
{
    float lifeTime : IN_LIFEINTE; //!< ¶‘¶ŽžŠÔ
    float speed : IN_SPEED; //!< ˆÚ“®‘¬“x
    float3 position : POSITION; //!< À•W
    float3 velocity : IN_VELOCITY; //!< ˆÚ“®—Ê
    float4 color : COLOR0; //!< F
};
    
struct GSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};


GSInput VSMain(VSInput input)
{
    GSInput result;

    result.position = input.position;

    result.color = input.color;

    return result;
}

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

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.col;
}
