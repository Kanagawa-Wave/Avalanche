#include "Layouts.hlsli"

struct LightSpaceMatrix
{
    float4x4 ViewProjection;
};

struct Constants
{
    float4x4 Model;
};

cbuffer lightSpaceMatrix : register(b0, space0)
{
    LightSpaceMatrix lightSpaceMatrix;
}

[[vk::push_constant]]
Constants constants;

VSOutput main(VSInput_PosColorNormalTex input)
{
    VSOutput output;
    output.Position = mul(lightSpaceMatrix.ViewProjection, mul(constants.Model, float4(input.Position, 1.0)));

    return output;
}