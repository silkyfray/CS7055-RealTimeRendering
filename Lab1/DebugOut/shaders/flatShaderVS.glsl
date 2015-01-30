#version 440
in vec3 position;

layout (location = 0) uniform mat4 M;
layout (location = 1) uniform mat4 V;
layout (location = 2) uniform mat4 P;

void main()
{
    gl_Position = P * V * M * vec4(position, 1.0f);
}