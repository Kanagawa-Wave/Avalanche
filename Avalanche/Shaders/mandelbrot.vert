#version 450

layout (location = 0) in vec3 vPosition;

layout (location = 0) out vec3 fPosition;

void main() {
    gl_Position = vec4(vPosition, 1.f);
    fPosition = vec3(vPosition.xy + 1.f, 0.0f);
}