struct VSInput
{
    float lifeTime : IN_LIFETIME; //!< ��������
    float speed : IN_SPEED; //!< �ړ����x
    float3 position : POSITION; //!< ���W
    float3 velocity : IN_VELOCITY; //!< �ړ���
    float4 color : COLOR0; //!< �F
};
    
struct GSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};


GSInput VSMain(VSInput input)
{
    GSInput result = (GSInput) 0;

    result.position = float4(input.position, 1.0f);

    result.color = input.color;

    return result;
}
