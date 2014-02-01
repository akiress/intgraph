#version 150

uniform mat4 M;
uniform mat4 P;

attribute vec4 my_Vertex;

void main () 
{
    gl_Position = P * M * my_Vertex;
} 