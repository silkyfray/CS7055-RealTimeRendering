#version 400
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

struct LightInfo {
	vec3 Direction; // Light position in eye coords.
	vec3 Intensity; // A,D,S intensity
};

uniform LightInfo Light;
out vec3 fsNormal;
out vec2 fsTexCoord;
out vec3 fsLightDir;
out vec3 fsViewDir;
out vec3 fsHalfVector;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
//uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
	fsTexCoord = texCoord;
	fsNormal = normal;

	vec3 n = normalize(NormalMatrix * normal);
	vec3 t = normalize(NormalMatrix * tangent);
	vec3 b = normalize(NormalMatrix * bitangent);

	vec3 vertexPosition = vec3(ModelViewMatrix * vec4(position,1.0));
	vec3 lightDir = normalize(Light.Direction);

	vec3 v;
	v.x = dot(lightDir, t);
	v.y = dot(lightDir, b);
	v.z = dot(lightDir, n);
	fsLightDir = normalize(v);

	v.x = dot(vertexPosition, t);
	v.y = dot(vertexPosition, b);
	v.z = dot(vertexPosition, n);
	fsViewDir = normalize(v);

	vertexPosition = normalize(vertexPosition);

	vec3 halfVector = normalize(vertexPosition + lightDir);
	v.x = dot(halfVector, t);
	v.y = dot(halfVector, b);
	v.z = dot(halfVector, n);

	fsHalfVector = v;

	gl_Position = MVP * vec4(position, 1.0);
}