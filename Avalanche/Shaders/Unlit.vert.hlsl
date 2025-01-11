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
};

[[vk::push_constant]]
Constants constants;

cbuffer camera : register(b0, space0) { Camera camera; }

VSOutput_ColorTexWNomralWPos main(VSInput_PosColorNormalTex input)
{
    VSOutput_ColorTexWNomralWPos output = (VSOutput_ColorTexWNomralWPos)0;
    output.Position = mul(camera.ViewProjection, mul(constants.Model, float4(input.Position, 1.0)));
    output.Color = input.Color;
    output.Texcoord = input.Texcoord;

    return output;
}
