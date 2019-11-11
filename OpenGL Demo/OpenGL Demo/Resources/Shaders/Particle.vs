#version 450 core

layout (location = 0) in vec3 vertex; 

void main(){ 
	// movement is in world space 
	gl_Position = vec4(vertex, 1.0f);
} 
