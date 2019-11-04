#version 430 core


out vec4 FragColor;
  
in vec2 TexCoords;
in vec2 fragCoord;

uniform sampler2D renderTexture;
uniform float currentTime;

const float offset = 1.0 / 300.0;  

void main()
{
	vec2 uv = TexCoords;
	float d = length(uv - vec2(0.5,0.5));
	
	float blur = 0.0;	
	blur = (1.0 + sin(currentTime*6.0)) * 0.5;
	blur *= 1.0 + sin(currentTime*16.0) * 0.5;
	blur = pow(blur, 3.0);
	blur *= 0.05;
	// reduce blur towards center
	blur *= d;
	
	vec3 col;
    col.r = texture(renderTexture, vec2(uv.x+blur,uv.y) ).r;
    col.g = texture(renderTexture, uv ).g;
    col.b = texture(renderTexture, vec2(uv.x-blur,uv.y) ).b;
	
	// scanline
	float scanline = sin(uv.y*800.0)*0.04;
	col -= scanline;
	
	// vignette
	col *= 1.0 - d * 0.5;
	
    FragColor = vec4(col,1.0);
	
}  