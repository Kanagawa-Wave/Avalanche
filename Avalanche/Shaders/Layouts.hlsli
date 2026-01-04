struct VSInput_PosColorNormalTex
{
    [[vk::location(0)]] float3 Position : POSITION;
    [[vk::location(1)]] float3 Color : COLOR;
    [[vk::location(2)]] float3 Normal : NORMAL;
    [[vk::location(3)]] float2 Texcoord : TEXCOORD;
};

struct VSOutput_ColorTexWNomralWPos
{
    float4 Position : SV_POSITION;
    [[vk::location(0)]] float3 Color : COLOR;
    [[vk::location(1)]] float2 Texcoord : TEXCOORD;
    [[vk::location(2)]] float3 WorldNormal : NORMAL;
    [[vk::location(3)]] float3 WorldPosition : POSITION;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
};

struct VSInput_Pos
{
    [[vk::location(0)]] float2 Position : POSITION;
};

struct VSOutput_OffsetTex
{
    float4 Position : SV_POSITION;
    [[vk::location(0)]] float2 Offset : TEXCOORD0;
    [[vk::location(1)]] float2 Texcoord : TEXCOORD1;
};