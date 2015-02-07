#version 400
 
uniform mat4 m_pvm;
uniform mat4 m_proj;
uniform mat3 m_normal;
 
in vec4 position;   // local space
in vec3 normal;     // local space

uniform float lineThickness;
void main () {
 
    vec4 normalScreen = normalize(m_proj * vec4(m_normal * normal, 1.0));
    vec4 original = m_pvm * position;
    gl_Position = original + lineThickness * normalScreen;
}