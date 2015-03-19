#version 440

uniform float mixRRfactor;
//reflect refract stuff
in vec3 Reflect;
in vec3 RefractR;
in vec3 RefractG;
in vec3 RefractB;
in float Ratio;
//THERE IS CURRENTLY A BUG IN ATI CARDS WHERE WE CANT MIX TWO TEXTURE TYPES IN THE SAME SHADER
//different sample types for same sample texture unit samplercube
layout(location = 4) uniform samplerCube Cubemap;
out vec4 FragColor;

//normal map stuff
in vec3 fsLightDir;
in vec2 fsTexCoord;
in vec3 fsViewDir;
in vec3 fsNormal;
in vec3 fsHalfVector;

layout(location = 1) uniform sampler2D ColorTex;
layout(location = 2) uniform sampler2D NormalMapTex;
layout(location = 3) uniform sampler2D LightMapTex;

struct LightInfo {
	vec3 Direction;
	vec3 Intensity; // A,D,S intensity
};
uniform LightInfo Light;
struct MaterialInfo {
	vec4 Ka; // Ambient reflectivity
	vec4 Ks; // Specular reflectivity
	float Shininess; // Specular shininess factor
};
uniform MaterialInfo Material;

void main()
{
	//normal map stuff
	vec4 normalColor = vec4(0.0);
	vec3 normal = 2.0 * texture(NormalMapTex, fsTexCoord).rgb - 1.0;
	normal = normalize(normal);

	//diffuse
	float lamberFactor = max(dot(fsLightDir, normal), 0.0);
	vec4 diffuseMaterial = vec4(0.0);
	vec4 diffuseLight = vec4(0.0);

	//specular
	vec4 specularMaterial;
	vec4 specularLight;
	float shininess;

	vec4 ambientLight = vec4(Light.Intensity, 1.0);
	vec4 ambientMaterial = Material.Ka * ambientLight;
	if(lamberFactor > 0.0)
	{
		diffuseMaterial = texture(ColorTex, fsTexCoord);
		diffuseLight = vec4(Light.Intensity, 1.0);

		specularMaterial = texture(LightMapTex, fsTexCoord);
		specularLight = vec4(Light.Intensity, 1.0);
		shininess = pow(max(dot(fsHalfVector, normal), 0.0), 3.0);

		normalColor = diffuseMaterial * diffuseLight * lamberFactor;
		normalColor += specularMaterial * specularLight * shininess;
	}

	normalColor += ambientMaterial;

	//reflect refract stuff
	vec3 refractColor;
	refractColor.r = texture(Cubemap, RefractR).r;
	refractColor.g = texture(Cubemap, RefractG).g;
	refractColor.b = texture(Cubemap, RefractB).b;
	vec3 reflectColor = vec3(texture(Cubemap, Reflect));
	vec4 rrColor = vec4(mix(refractColor, reflectColor, Ratio), 1.0);
	//vec4 rrColor = vec4(0.0);



	FragColor = mix(normalColor, rrColor, mixRRfactor);

	//FragColor = rrColor;
}