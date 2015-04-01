#version 400
 
uniform vec4 diffuse;
uniform vec4 ambient;
uniform vec3 l_dir;    // camera space
 
in Data {
    vec3 normal;
    vec4 eye;
} DataIn;
 
out vec4 colorOut;
 
void main() {
 
    vec4 finalDiffuse = diffuse;
    // normalize both input vectors
    vec3 n = normalize(DataIn.normal);
    vec3 e = normalize(vec3(DataIn.eye));
    vec3 l = normalize(l_dir);
    float intensity = max(dot(n,l), 0.0);
 
    // if the vertex is lit compute the specular color
    if (intensity > 0.0) {
	    // Discretize the intensity, based on a few cutoff points
	    if(intensity > 0.98f)
            intensity = 1.0f;
		else if (intensity > 0.5f)
		    intensity = 0.7f;
		else if (intensity > 0.05)
		    intensity = 0.35f;
		else
		    intensity = 0.01f;
    }
    colorOut = max(intensity *  finalDiffuse, 0.2 * finalDiffuse);
    //colorOut = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}