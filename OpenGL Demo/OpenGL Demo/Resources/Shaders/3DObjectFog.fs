#version 450 core

in vec2 fragTexCoord;
in vec4 mWorldPos;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform vec3 camPos;

void main()
{
   vec4 baseColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
   float d = distance(mWorldPos.xyz, camPos); 
   float lerp = (d - 5.0f)/10.f; 
   lerp = clamp(lerp, 0.0, 1.0);
   vec4 vFogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
   color = mix(baseColor, vFogColor, lerp);

}