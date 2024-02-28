#version 450

#define PI 3.1415926538

layout (location = 0) in vec3 fPosition;

layout (location = 0) out vec4 outColor;



void main() {
    const vec2 c = fPosition.xy;
    vec2 z = vec2(0, 0);
    for (int i = 0; i < 100; i++)
    {
        float x = z.x;
        float y = z.y;
        z = vec2(x*x-y*y-c.x, 2*x*y+c.y);
    }
    outColor = vec4(vec3((atan(length(z))+PI/2)/PI), 1.0);
}