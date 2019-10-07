#version 430 core 

layout (points) in;
layout (line_strip, max_vertices = 11) out;
out vec3 outColor;

in VS_GS_VERTEX{ 

 in vec4 position;
 in vec3 color; 
 in mat4 mvp; 
 
}gs_in[];

void main() {   
 
	outColor = gs_in[0].color;
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.15f, 0.25, 0.0f, 0.0f);
	EmitVertex();
	
	//top point
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.0f, 0.75f, 0.0f, 0.0f);
	EmitVertex();
	
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(-0.15f, 0.25f, 0.0f, 0.0f);
	EmitVertex();
	
	//left top
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(-0.5f, 0.35f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(-0.25f, -0.15f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(-0.35f, -0.75f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(-0.0f, -0.35f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.35f, -0.75f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.25f, -0.15f, 0.0f, 0.0f);
	EmitVertex();
	
		//left top
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.5f, 0.35f, 0.0f, 0.0f);
	EmitVertex();
	
	gl_Position = gs_in[0].position + gs_in[0].mvp * vec4(0.15f, 0.25, 0.0f, 0.0f);
	EmitVertex();
	

	
	EndPrimitive();
}