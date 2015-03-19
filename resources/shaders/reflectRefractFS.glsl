#version 400
in vec3 ReflectDir;
in vec3 RefractDir;
uniform samplerCube CubeMapTex;
uniform bool DrawSkyBox;

struct MaterialInfo {
	float Eta; // Ratio of indices of refraction
	float ReflectionFactor; // Percentage of reflected light
};
uniform MaterialInfo Material;
layout( location = 0 ) out vec4 FragColor;
void main() {
	// Access the cube map texture
	vec4 reflectColor = texture(CubeMapTex, ReflectDir);
	vec4 refractColor = texture(CubeMapTex, RefractDir);
	if( DrawSkyBox )
		FragColor = reflectColor;
	else
		FragColor = mix(refractColor, reflectColor,	Material.ReflectionFactor);

		//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}