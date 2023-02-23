#version 460 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inColor;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec2 inTexCoord;

layout (location = 0) out vec3 outColor;
layout (location = 1) out vec2 outTexCoord;
layout (location = 2) out vec3 outPosWorld;
layout (location = 3) out vec3 outNormalWorld;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 normal;

layout (set = 0, binding = 0) uniform GlobalUbo 
{
    mat4 projection;
    mat4 view;
    vec4 ambientColor;
    vec3 lightPosition;
    vec4 lightColor; // w is lightIntensity
} ubo;

void main() {
    gl_Position = ubo.projection * ubo.view * model * vec4(inPos, 1.0);
    
    outNormalWorld = normalize(mat3(normal) * inNormal);
    outPosWorld = (model * vec4(inPos, 1.0)).xyz;
    outColor = inColor;
    outTexCoord = inTexCoord;
}