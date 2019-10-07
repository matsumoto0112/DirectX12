#define THREAD_X 1
#define THREAD_Y 1
#define THREAD_Z 1

#define DISPATCH_X 1
#define DISPATCH_Y 1
#define DISPATCH_Z 1

RWStructuredBuffer<float> data : register(u0);


struct CSInput
{
    uint3 groupThread : SV_GroupThreadID;
    uint3 group : SV_GroupID;
    uint groupIndex : SV_GroupIndex;
    uint3 dispatch : SV_DispatchThreadID;
};

[numthreads(THREAD_X, THREAD_Y, THREAD_Z)]
void main(CSInput input)
{
    data[input.dispatch.x] = input.dispatch.x;
}