#version 440
in vec3 vertex_pos;
in vec4 vertex_color;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
out vec4 frag_color;
void main(){
	frag_color = vertex_color;
	gl_Position = P * V * M *  vec4(vertex_pos,1.0);
}