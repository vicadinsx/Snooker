#version 330 core

in vec3 in_Position;
in vec2 in_TexCoords;
in vec3 in_Normal;

out vec2 ex_TexCoords;
out vec3 ex_Normal;
out vec3 fEye;
out vec3 fLight;
out vec3 vertPos;
out vec4 aProduct, dProduct, sProduct;

uniform mat4 Matrix;
uniform vec4 LightPosition;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;

uniform Camera
{
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
};

void main(void)
{
    gl_Position = ProjectionMatrix * ViewMatrix * Matrix * vec4(in_Position,1);

    ex_TexCoords = in_TexCoords;

	vec4 vertPos4 = Matrix * vec4(in_Position, 1);

    fEye = vertPos4.xyz;
    fLight = LightPosition.xyz;

	vertPos = vec3(vertPos4) / vertPos4.w;

    ex_Normal = vec3(Matrix * vec4(in_Normal, 0.0));

	aProduct = AmbientProduct;
	dProduct = DiffuseProduct;
	sProduct = SpecularProduct;
}
