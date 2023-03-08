#version 450

vec3 positions[] = {vec3(1.f), vec3(1.f), vec3(1.f)};

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 1.f);
}