#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D screenTexture;

void main()
{
	float factor = 0.822;
	float r = dot(texture(screenTexture, TexCoords).xyz, vec3(0.393, 0.769, 0.189));
	float g = dot(texture(screenTexture, TexCoords).xyz, vec3(0.349, 0.686, 0.168));
	float b = dot(texture(screenTexture, TexCoords).xyz, vec3(0.272, 0.534, 0.131));
	color  = vec4(r*factor, g*factor, b*factor, 1.0) + vec4(0.055, 0.0, 0.0, 1.0) ;
} 