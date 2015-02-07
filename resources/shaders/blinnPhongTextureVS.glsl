#version 400
 
uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat3 m_normal;
 
in vec4 position;   // local space
in vec3 normal;     // local space
in vec2 texCoord;     // local space
 
// the data to be sent to the fragment shader
out Data {
    vec3 normal;
    vec4 eye;
    vec2 texCoord;
} DataOut;
 
void main () {
 
	DataOut.normal = normalize(m_normal * normal);
	DataOut.eye = -(m_viewModel * position);
	DataOut.texCoord = texCoord;
	gl_Position = m_pvm * position;

}