#version 460 core

layout (location = 0) in vec3 inPos;

layout (location = 0) out vec3 outTexCoord;

layout (set = 0, binding = 0) uniform GlobalUbo
{
    mat4 projection;
    mat4 view;
    vec4 ambientColor;
    vec3 lightPosition;
    vec4 lightColor; // w is lightIntensity
} ubo;

void main() {
    gl_Position = ubo.projection * mat4(mat3(ubo.view)) * vec4(inPos, 1.0);
    outTexCoord = inPos;
}