#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300;  

void main()
{
	float gray = dot(texture(screenTexture, TexCoords).xyz, vec3(0.299, 0.587, 0.114));
	color  = gray * vec4(1.2, 1.0, 0.8, 1.0);
} 