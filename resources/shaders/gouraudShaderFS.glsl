#version 440
 
in Data {
    vec4 color;
} DataIn;
 
out vec4 outputF;
 
void main() {
 
    outputF = DataIn.color;
    //outputF = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}