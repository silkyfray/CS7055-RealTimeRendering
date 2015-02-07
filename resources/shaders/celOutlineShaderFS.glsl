#version 400

uniform vec4 lineColor; 
// the data to be sent to the fragment shader
out vec4 color;
 
void main () {
	color = lineColor;
}