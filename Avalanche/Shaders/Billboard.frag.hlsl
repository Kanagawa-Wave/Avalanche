#include "Layouts.hlsli"

[[vk::combinedImageSampler]]
Texture2D<float4> baseColor : register(t0, space1);
[[vk::combinedImageSampler]]
SamplerState baseColorSampler : register(s0, space1);

float4 main(VSOutput_Tex input) : SV_TARGET
{
    const float4 texColor = baseColor.Sample(baseColorSampler, input.Texcoord);

    return texColor;
}