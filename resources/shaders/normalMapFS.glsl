#version 400
in vec3 fsLightDir;
in vec2 fsTexCoord;
in vec3 fsViewDir;
in vec3 fsNormal;
in vec3 fsHalfVector;

uniform sampler2D ColorTex;
uniform sampler2D NormalMapTex;
uniform sampler2D LightMapTex;

uniform bool renderDiffuse;
uniform bool renderNormalMap;

struct LightInfo {
	vec4 Position; // Light position in eye coords.
	vec3 Intensity; // A,D,S intensity
};
uniform LightInfo Light;
struct MaterialInfo {
	vec4 Ka; // Ambient reflectivity
	vec4 Ks; // Specular reflectivity
	float Shininess; // Specular shininess factor
};
uniform MaterialInfo Material;
layout( location = 0 ) out vec4 FragColor;



void main() {
	vec3 normal = 2.0 * texture(NormalMapTex, fsTexCoord).rgb - 1.0;
	normal = normalize(normal);
	FragColor = vec4(0.0);
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
		if(renderDiffuse)
		{
			diffuseMaterial = texture(ColorTex, fsTexCoord);
			diffuseLight = vec4(Light.Intensity, 1.0);
			FragColor = diffuseMaterial * diffuseLight * lamberFactor;
			FragColor += ambientMaterial;
		}
		if(renderNormalMap)
		{
			if(!renderDiffuse)
				FragColor = vec4(0.0, 0.2, 0.2, 1.0);
			specularMaterial = texture(LightMapTex, fsTexCoord);
			specularLight = vec4(Light.Intensity, 1.0);
			shininess = pow(max(dot(fsHalfVector, normal), 0.0), 3.0);
			FragColor += specularMaterial * specularLight * shininess;
		}
	}


} 