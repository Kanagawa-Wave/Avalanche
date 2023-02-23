#version 460 core

layout (location = 0) in vec3 inColor;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inPosWorld;
layout (location = 3) in vec3 inNormalWorld;

layout (location = 0) out vec4 fragColor;

layout (binding = 0) uniform sampler2D tex;

layout (set = 0, binding = 0) uniform GlobalUbo
{
    mat4 projection;
    mat4 view;
    vec4 ambientColor;
    vec3 lightPosition;
    vec4 lightColor; // w is lightIntensity
} ubo;

void main() {
    vec3 directionToLight = ubo.lightPosition - inPosWorld;
    float attenuation = 1.0 / dot(directionToLight, directionToLight);
    vec3 lightColor = ubo.lightColor.xyz * ubo.lightColor.w * attenuation;

    vec3 ambient = ubo.ambientColor.xyz * ubo.ambientColor.w;
    vec3 diffuse = lightColor * max(dot(inNormalWorld, normalize(directionToLight)), 0);
     
    fragColor = texture(tex, inTexCoord) * vec4((diffuse + ambient) * inColor, 0);
}