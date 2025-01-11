#include "Layouts.hlsli"

struct Material
{
    float Test;
};

cbuffer material : register(b1, space0) { Material material; }

[[vk::combinedImageSampler]]
Texture2D<float4> baseColor : register(t0, space1);
[[vk::combinedImageSampler]]
SamplerState baseColorSampler : register(s0, space1);

float4 main(VSOutput_ColorTexWNomralWPos input) : SV_TARGET
{
    float4 color = baseColor.Sample(baseColorSampler, input.Texcoord) * float4(input.Color, 1.0);
    return color;
}