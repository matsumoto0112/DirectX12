struct VSInput
{
    float lifeTime : IN_LIFETIME; //!< 生存時間
    float speed : IN_SPEED; //!< 移動速度
    float3 position : POSITION; //!< 座標
    float3 velocity : IN_VELOCITY; //!< 移動量
    float4 color : COLOR0; //!< 色
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
