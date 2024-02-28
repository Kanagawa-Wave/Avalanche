#include "Layouts.hlsli"

struct PointLight
{
    float3 Position;
    float3 Color;
};

struct Camera
{
    float3 Position;
};

cbuffer pointlight : register(b1, space0)
{
    PointLight pointlight;
}

cbuffer camera : register(b2, space0)
{
    Camera camera;
}

[[vk::combinedImageSampler]]
Texture2D<float4> baseColor : register(t0, space1);
[[vk::combinedImageSampler]]
SamplerState baseColorSampler : register(s0, space1);

float4 main(VSOutput input) : SV_TARGET
{
    const float specularStrength = 0.5f;

    const float3 N = normalize(input.WorldNormal);

    // Diffuse
    const float3 lightDir = normalize(pointlight.Position - input.WorldPosition);
    const float3 diffuse = pointlight.Color * max(dot(N, lightDir), 0.0);

    // Specular
    const float3 viewDir = normalize(camera.Position - input.WorldPosition);
    const float3 reflectDir = reflect(-lightDir, N);
    const float3 specular = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), 32) * pointlight.Color;

    const float4 albedo = baseColor.Sample(baseColorSampler, input.Texcoord);

    float4 color = (float4(specular, 1.0) + float4(diffuse, 1.0)) * albedo;

    return color;
}