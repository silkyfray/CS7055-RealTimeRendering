#version 400

in vec3 Reflect;
in vec3 RefractR;
in vec3 RefractG;
in vec3 RefractB;
in float Ratio;
uniform samplerCube Cubemap;
out vec4 FragColor;
void main()
{
	vec3 refractColor;
	refractColor.r = texture(Cubemap, RefractR).r;
	refractColor.g = texture(Cubemap, RefractG).g;
	refractColor.b = texture(Cubemap, RefractB).b;
	vec3 reflectColor = vec3(texture(Cubemap, Reflect));
	vec3 color = mix(refractColor, reflectColor, Ratio);
	FragColor = vec4(color, 1.0);
}