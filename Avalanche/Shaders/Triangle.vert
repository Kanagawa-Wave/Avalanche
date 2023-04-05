#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec2 vTexcoord;

layout (location = 0) out vec3 fColor;

layout (push_constant) uniform Constants
{
    mat4 model;
} constants;

layout (set = 0, binding = 0) uniform Camera {
    mat4 projection;
    mat4 view;
    mat4 viewProjection;
} camera;

layout (set = 0, binding = 0) uniform Material {
    float test;
} material;

void main() {
    gl_Position = camera.viewProjection * constants.model * vec4(vPosition, 1.0) ;
    fColor = vec3(material.test, 0.0, 0.0);
}