#version 400
layout (location = 0) in vec3 position;

out vec3 ReflectDir; // Reflected direction
uniform mat4 MVP;

void main()
{
	ReflectDir = position;
	gl_Position = MVP * vec4(position, 1.0);
}