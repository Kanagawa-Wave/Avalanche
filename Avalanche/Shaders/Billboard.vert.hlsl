#include "Layouts.hlsli"

struct Camera
{
    float4x4 Projection;
    float4x4 View;
    float4x4 ViewProjection;
};

struct Constants
{
    float3 Position;
};

[[vk::push_constant]]
Constants constants;

cbuffer camera : register(b0, space0)
{
    Camera camera;
}

VSOutput_OffsetTex main(VSInput_Pos input)
{
    const float BILLBOARD_SIZE = 0.1;
    
    VSOutput_OffsetTex output;

    float3 cameraRightWorld = normalize(float3(camera.View[0][0], camera.View[0][1], camera.View[0][2]));
    float3 cameraUpWorld = normalize(float3(camera.View[1][0], camera.View[1][1], camera.View[1][2]));

    float3 worldPosition =
        constants.Position + (BILLBOARD_SIZE * input.Position.x * cameraRightWorld) + (BILLBOARD_SIZE * input.Position.y * cameraUpWorld);

    output.Position = mul(camera.ViewProjection, float4(worldPosition, 1.0));
    output.Offset = input.Position;
    output.Texcoord = 1 - (input.Position * 0.5 + 0.5);

    return output;
}
