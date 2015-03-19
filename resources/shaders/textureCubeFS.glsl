#version 400
in vec3 ReflectDir;
uniform samplerCube Cubemap;
layout( location = 0 ) out vec4 FragColor;

void main() {
	// Access the cube map texture
	vec4 reflectColor = texture(Cubemap, ReflectDir);
	FragColor = reflectColor;

}