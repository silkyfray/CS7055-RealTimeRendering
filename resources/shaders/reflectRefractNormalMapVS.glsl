#version 440
in vec3 position;
in vec3 normal;

//reflect refract stuff
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

//normal map stuff
in vec2 texCoord;
in vec3 tangent;
in vec3 bitangent;

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

void main()
{
	//reflect refract stuff
	vec3 i = normalize(vec3(ModelViewMatrix * vec4(position,1.0)));
	vec3 n = normalize(NormalMatrix * normal);
	Ratio = F + (1.0 - F) * pow((1.0 - dot(-i, n)), FresnelPower);
	RefractR = refract(i, n, Eta.r);
	RefractG = refract(i, n, Eta.g);
	RefractB = refract(i, n, Eta.b);

	Reflect = reflect(i, n);

	gl_Position = MVP * vec4(position, 1.0);

	//normal map stuff
	fsTexCoord = texCoord;
	fsNormal = normal;

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
}