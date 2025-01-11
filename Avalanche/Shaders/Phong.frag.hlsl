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

cbuffer pointlights : register(b1, space0)
{
    PointLight pointlights[16];
    uint numLights;
}

cbuffer camera : register(b2, space0)
{
    Camera camera;
}

[[vk::combinedImageSampler]]
Texture2D<float4> baseColor : register(t0, space1);
[[vk::combinedImageSampler]]
SamplerState baseColorSampler : register(s0, space1);

float4 main(VSOutput_ColorTexWNomralWPos input) : SV_TARGET
{
    const float specularStrength = 0.5f;

    const float3 N = normalize(input.WorldNormal);

    float3 diffuse = float3(0, 0, 0);
    float3 specular = float3(0, 0, 0);

    for (int i = 0; i < numLights; i++)
    {
        // Diffuse
        const float3 lightDir = normalize(pointlights[i].Position - input.WorldPosition);
        diffuse += pointlights[i].Color * max(dot(N, lightDir), 0.0);

        // Specular
        const float3 viewDir = normalize(camera.Position - input.WorldPosition);
        const float3 reflectDir = reflect(-lightDir, N);
        specular = specularStrength * pow(max(dot(viewDir, reflectDir), 0.0), 32) * pointlights[i].Color;
    }
    
    const float4 albedo = baseColor.Sample(baseColorSampler, input.Texcoord);

    float4 color = (float4(specular, 1.0) + float4(diffuse, 1.0)) * albedo;

    return color;
}