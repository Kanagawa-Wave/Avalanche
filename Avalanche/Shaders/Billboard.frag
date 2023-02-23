#version 460

layout (location = 0) in vec2 inOffset;

layout (location = 0) out vec4 fragColor;

layout (set = 0, binding = 0) uniform GlobalUbo
{
    mat4 projection;
    mat4 view;
    vec4 ambientColor;
    vec3 lightPosition;
    vec4 lightColor; // w is lightIntensity
} ubo;

void main() {
    float dis = sqrt(dot(inOffset, inOffset));
//    if (dis >= 1.f)
//    {
//        discard;
//    }
    fragColor = vec4(ubo.lightColor.xyz, 1.0);
}