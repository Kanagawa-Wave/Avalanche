#include "Layouts.hlsli"

struct PointLight
{
    float3 Position;
    float3 Color;
};

cbuffer pointlight : register(b1, space0)
{
    PointLight pointlight;
}

[[vk::combinedImageSampler]]
Texture2D<float4> baseColor : register(t0, space1);
[[vk::combinedImageSampler]]
SamplerState baseColorSampler : register(s0, space1);

float4 main(VSOutput input) : SV_TARGET
{
    const float3 N = normalize(input.WorldNormal);
    const float3 lightDir = normalize(pointlight.Position - input.WorldPosition);
    const float3 diffuse = pointlight.Color * max(dot(N, lightDir), 0.0);

    const float4 albedo = baseColor.Sample(baseColorSampler, input.Texcoord) * float4(input.Color, 1.0);
    float4 color = float4(diffuse, 1.0) * albedo;

    return color;
}