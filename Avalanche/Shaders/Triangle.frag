#version 450

layout (location = 0) in vec3 fColor;
layout (location = 1) in vec2 fTexcoord;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 1) uniform Material {
    float test;
} material;

layout (set = 1, binding = 0) uniform sampler2D baseColor;

void main() {
    outColor = texture(baseColor, fTexcoord);
    // outColor = vec4(fColor, 1.0);
}