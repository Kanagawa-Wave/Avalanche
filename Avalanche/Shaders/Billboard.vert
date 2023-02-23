#version 460

layout (location = 0) in vec2 inPos;

layout (set = 0, binding = 0) uniform GlobalUbo
{
    mat4 projection;
    mat4 view;
    vec4 ambientColor;
    vec3 lightPosition;
    vec4 lightColor; // w is lightIntensity
} ubo;

layout (location = 0) out vec2 outOffset;

const float RADIUS = 0.05f;

void main() {
    outOffset = inPos;
    vec3 cameraRightWorld = {ubo.view[0][0], ubo.view[1][0], ubo.view[2][0]};
    vec3 cameraUpWorld = {ubo.view[0][1], ubo.view[1][1], ubo.view[2][1]};

    vec3 positionWorld = ubo.lightPosition.xyz + RADIUS * outOffset.x * cameraRightWorld + RADIUS * outOffset.y * cameraUpWorld;
    gl_Position = ubo.projection * ubo.view * vec4(positionWorld, 1.0);
}