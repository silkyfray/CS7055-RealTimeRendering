#version 400
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform vec3 Eta; // Ratio of indices of refraction
uniform float FresnelPower;

uniform float F; //((1.0-Eta) * (1.0-Eta)) / ((1.0+Eta) * (1.0+Eta))

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

out vec3 Reflect;
out vec3 RefractR;
out vec3 RefractG;
out vec3 RefractB;
out float Ratio;

void main()
{
	vec3 i = normalize(vec3(ModelViewMatrix * vec4(position,1.0)));
	vec3 n = normalize(NormalMatrix * normal);
	Ratio = F + (1.0 - F) * pow((1.0 - dot(-i, n)), FresnelPower);
	RefractR = refract(i, n, Eta.r);
	RefractG = refract(i, n, Eta.g);
	RefractB = refract(i, n, Eta.b);

	Reflect = reflect(i, n);

	gl_Position = MVP * vec4(position, 1.0);
}