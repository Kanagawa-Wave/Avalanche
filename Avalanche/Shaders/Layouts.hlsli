struct VSInput
{
    [[vk::location(0)]] float3 Position : POSITION;
    [[vk::location(1)]] float3 Color : COLOR;
    [[vk::location(2)]] float3 Normal : NORMAL;
    [[vk::location(3)]] float2 Texcoord : TEXCOORD;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    [[vk::location(0)]] float3 Color : COLOR;
    [[vk::location(1)]] float2 Texcoord : TEXCOORD;
    [[vk::location(2)]] float3 WorldNormal : NORMAL;
    [[vk::location(3)]] float3 WorldPosition : POSITION;
};