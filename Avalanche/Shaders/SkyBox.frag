#version 460 core

layout (set = 0, binding = 0) uniform GlobalUbo
{
    mat4 projection;
    mat4 view;
    vec4 ambientColor;
    vec3 lightPosition;
    vec4 lightColor; // w is lightIntensity
} ubo;

layout (location = 0) in vec3 inTexCoord;

layout (location = 0) out vec4 fragColor;

layout (binding = 0) uniform samplerCube tex;

void main() {
    fragColor = texture(tex, inTexCoord);
}