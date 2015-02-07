#version 440

    
uniform mat4 m_pvm;  
uniform mat3 m_normal;
uniform mat4 m_viewModel;

uniform vec3 l_dir;     

uniform vec4 diffuse;
uniform vec4 ambient;
uniform vec4 specular;
uniform float shininess;

uniform vec4 diffuseLight = vec4(0.8f);
uniform vec4 specularLight = vec4(0.5f);
uniform vec4 ambientLight = vec4(1.0f);

in vec4 position;   // local space
in vec3 normal;     // local space
 
// the data to be sent to the fragment shader
out Data {
    vec4 color;
} DataOut;
 
void main () {
 

    // set the specular term initially to black
    vec4 spec = vec4(0.0);
 
    vec3 n = normalize(m_normal * normal);
 
    float intensity = max(dot(n, l_dir), 0.0);
 
    // if the vertex is lit compute the specular term
    if (intensity > 0.0) {
        // compute position in camera space
        vec3 pos = vec3(m_viewModel * position);
        // compute eye vector and normalize it 
        vec3 eye = normalize(-pos);
        // compute the half vector
        vec3 h = normalize(l_dir + eye);
 
        // compute the specular term into spec
        float intSpec = max(dot(h,n), 0.0);
        spec = specular * pow(intSpec, shininess);
    }   
    vec4 finalDiffuse = (diffuseLight * diffuse) * intensity;
    vec4 finalSpecular = specularLight * spec;
    vec4 finalAmbient = ambientLight * ambient;
    // add the specular term 
    DataOut.color = max(finalDiffuse + finalSpecular, finalAmbient);
 
    gl_Position = m_pvm * position; 
}