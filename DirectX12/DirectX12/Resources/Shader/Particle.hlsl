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

#define THREAD_X 128
#define THREAD_Y 128

#define DISPATCH_X 1
#define DISPATCH_Y 1

struct CSInput
{
    uint3 groupThread : SV_GroupThreadID;
    uint3 group : SV_GroupID;
    uint groupIndex : SV_GroupIndex;
    uint3 dispatch : SV_DispatchThreadID;
};

struct Particle
{
    float lifeTime; //!< ¶‘¶ŽžŠÔ
    float speed; //!< ˆÚ“®‘¬“x
    float3 position; //!< À•W
    float3 velocity; //!< ˆÚ“®—Ê
    float4 color; //!< F
};

#define LIFETIME_OFFSET (4 * 0)
#define SPEED_OFFSET (4 * 1)
#define POSITION_OFFSET (4 * 2)
#define VELOCITY_OFFSET (4 * 5)
#define COLOR_OFFSET (4 * 8)
#define PARTICLE_SIZE (4 * 12)


RWByteAddressBuffer particles : register(u0);

float getLifeTime(int index)
{
    return asfloat(particles.Load(index + LIFETIME_OFFSET));
}

float3 getPosition(int index)
{
    return asfloat(particles.Load3(index + POSITION_OFFSET));
}

float3 getVelocity(int index)
{
    return asfloat(particles.Load3(index + VELOCITY_OFFSET));
}

float getSpeed(int index)
{
    return asfloat(particles.Load(index + SPEED_OFFSET));
}

void updateParticle(int index)
{
    float3 newPos = getPosition(index) + getVelocity(index) * getSpeed(index) * 0.01f;
    particles.Store3(index + POSITION_OFFSET, asuint(newPos));
    float life = getLifeTime(index) - 0.01f;
    particles.Store(index + LIFETIME_OFFSET, asuint(life));
    if (life <= 0.0f)
        particles.Store(index + COLOR_OFFSET + 4 * 3, asuint(0.0f));
};



[numthreads(threadBlockSize, 1, 1)]
void CSMain(const CSInput input)
{
    const uint index = input.dispatch.z * DISPATCH_X * THREAD_X * DISPATCH_Y * THREAD_Y
        + input.dispatch.y * DISPATCH_X * THREAD_X
        + input.dispatch.x;

    const uint addr = index * PARTICLE_SIZE;

    if (getLifeTime(addr) > 0)
    {
        updateParticle(addr);
    }
    else
    {
        particles.Store4(addr + COLOR_OFFSET, asuint(float4(1.0f, 0.0f, 0.0f, 1.0f)));
    }
}
