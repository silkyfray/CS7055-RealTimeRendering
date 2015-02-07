#version 400
 

uniform vec4 specular;
uniform sampler2D texture_diffuse1;
uniform vec4 diffuse;
uniform vec4 ambient;
uniform float shininess;
uniform vec3 l_dir;    // camera space
 
in Data {
    vec3 normal;
    vec4 eye;
    vec2 texCoord;
} DataIn;
 
out vec4 colorOut;

void main() {
 
    // set the specular term to black
    vec4 spec = vec4(0.0);
 
    // normalize both input vectors
    vec3 n = normalize(DataIn.normal);
    vec3 e = normalize(vec3(DataIn.eye));
 
    float intensity = max(dot(n,l_dir), 0.0);
 
    // if the vertex is lit compute the specular color
    if (intensity > 0.0) {
        // compute the half vector
        vec3 h = normalize(l_dir + e);  
        // compute the specular term into spec
        float intSpec = max(dot(h,n), 0.0);
        spec = specular * pow(intSpec,shininess);
    }
    // colorOut = max(intensity *  vec4(texture(texture_diffuse1, DataIn.texCoord)) + spec, ambient);
    colorOut = max(intensity *  vec4(texture(texture_diffuse1, DataIn.texCoord)) + spec, ambient);
    //colorOut = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}