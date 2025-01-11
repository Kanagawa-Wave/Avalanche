#include "Layouts.hlsli"

struct Camera
{
    float4x4 Projection;
    float4x4 View;
    float4x4 ViewProjection;
};

struct Constants
{
    float4x4 Model;
    float4x4 NormalMat;
};

// maximum of 128byte (2 mat4) for best compatibility 
[[vk::push_constant]]
Constants constants;

cbuffer camera : register(b0, space0)
{
    Camera camera;
}

VSOutput_ColorTexWNomralWPos main(VSInput_PosColorNormalTex input)
{
    VSOutput_ColorTexWNomralWPos output = (VSOutput_ColorTexWNomralWPos) 0;
    output.Position = mul(camera.ViewProjection, mul(constants.Model, float4(input.Position, 1.0)));
    output.WorldPosition = float3(mul(constants.Model, float4(input.Position, 1.0)).xyz);
    output.Color = input.Color;
    output.Texcoord = input.Texcoord;
    output.WorldNormal = mul((float3x3)constants.NormalMat, input.Normal);

    return output;
}
