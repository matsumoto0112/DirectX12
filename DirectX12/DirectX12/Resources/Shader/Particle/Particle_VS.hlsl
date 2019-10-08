struct VSInput
{
    float lifeTime : IN_LIFETIME; //!< ¶‘¶ŽžŠÔ
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
    GSInput result = (GSInput) 0;

    result.position = float4(input.position, 1.0f);

    result.color = input.color;

    return result;
}
