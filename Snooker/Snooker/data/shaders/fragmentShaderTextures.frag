#version 330 core

in vec2 ex_TexCoords;
in vec3 ex_Normal;
in vec3 ex_texture;

out vec3 out_Color;
uniform sampler2D Texture;

void main(void)
{
    out_Color = texture(Texture, ex_TexCoords).rgb;
}
