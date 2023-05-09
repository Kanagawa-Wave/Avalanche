#version 450

layout (location = 0) in vec3 fColor;

layout (location = 0) out vec4 outColor;

layout (set = 0, binding = 1) uniform Material {
    float test;
} material;

void main() {
    outColor = vec4(material.test, 0.0, 0.0, 1.0);
}