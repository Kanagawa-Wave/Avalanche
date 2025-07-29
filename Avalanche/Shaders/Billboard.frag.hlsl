#include "Layouts.hlsli"

[[vk::combinedImageSampler]]
Texture2D<float4> baseColor : register(t0, space1);
[[vk::combinedImageSampler]]
SamplerState baseColorSampler : register(s0, space1);

float4 main(VSOutput_Offset input) : SV_TARGET
{
    float dst = sqrt(dot(input.Offset, input.Offset));
    float4 color = baseColor.Sample(baseColorSampler, input.Texcoord);
    
    if (color.a <= 0.001)
    {
        discard;
    }
    
    return color;
}