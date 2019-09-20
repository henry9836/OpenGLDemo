#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

out vec2 fragTexCoord;
out vec4 mWorldPos;

uniform mat4 proj_calc;
uniform mat4 model;
uniform mat4 view;

void main(void)
{
	mWorldPos = model *vec4(position, 1.0); 
	
	gl_Position =  proj_calc * view * mWorldPos;

	fragTexCoord = texCoord;
}