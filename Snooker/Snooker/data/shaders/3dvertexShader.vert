#version 330 core

in vec4 in_Position;
in vec2 vertexUV;
out vec2 UV;

uniform mat4 Matrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main(void)
{
	gl_Position = ProjectionMatrix * ViewMatrix * Matrix * in_Position;
	UV = vertexUV;
}
