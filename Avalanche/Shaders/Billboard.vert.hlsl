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

VSOutput_Tex main(VSInput_Pos input)
{
    const float BILLBOARD_SIZE = 1;
    
    VSOutput_Tex output = (VSOutput_Tex) 0;

    float3 cameraRightWorld = float3(camera.View[0][0], camera.View[1][0], camera.View[2][0]);
    float3 cameraUpWorld = float3(camera.View[0][1], camera.View[1][1], camera.View[2][1]);
    float3 billboardPosition = float3(constants.Model[3][0], constants.Model[3][1], constants.Model[3][2]);

    float3 worldPosition =
        billboardPosition + (BILLBOARD_SIZE * input.Position.x * cameraRightWorld) + (BILLBOARD_SIZE * input.Position.y * cameraUpWorld);

    output.Position = mul(camera.ViewProjection, float4(worldPosition, 1.0));
    output.Texcoord = input.Position * 0.5 + 0.5;

    return output;
}
