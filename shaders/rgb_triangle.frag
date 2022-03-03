//glsl version 4.5
#version 450

//output write
layout (location = 0) out vec4 outFragColor;
layout (location = 0) in vec4 inFragColor;

void main(){
    //return red
    outFragColor = inFragColor;
}
