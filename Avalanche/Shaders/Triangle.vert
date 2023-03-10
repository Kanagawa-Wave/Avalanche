#version 450

layout (location = 0) in vec3 vPosition;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec3 vNormal;
layout (location = 3) in vec2 vTexcoord;

layout (location = 0) out vec3 fColor;

layout (push_constant) uniform constants
{
    mat4 transform;
} PushConstants;

void main() {
    gl_Position = PushConstants.transform * vec4(vPosition, 1.0) ;
    fColor = vColor;
}