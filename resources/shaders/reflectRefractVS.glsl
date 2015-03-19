#version 400
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 ReflectDir; // Reflected direction
out vec3 RefractDir; // Transmitted direction
struct MaterialInfo {
	float Eta; // Ratio of indices of refraction
	float ReflectionFactor; // Percentage of reflected light
};
uniform MaterialInfo Material;
uniform bool DrawSkyBox;
uniform vec3 WorldCameraPosition;
uniform mat4 ModelViewMatrix;
uniform mat4 ModelMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;
void main()
{
	vec3 worldPos = vec3(ModelMatrix * vec4(position, 1.0));

	if( DrawSkyBox ) {
		ReflectDir = position;
	} else {
		vec3 worldPos = vec3(ModelMatrix * vec4(position, 1.0));
		vec3 worldNorm = vec3(NormalMatrix * normal);
		vec3 worldView = normalize(WorldCameraPosition - worldPos);
		ReflectDir = reflect(-worldView, worldNorm );
		RefractDir = refract(-worldView, worldNorm,	Material.Eta );
	}
	gl_Position = MVP * vec4(position, 1.0);
}