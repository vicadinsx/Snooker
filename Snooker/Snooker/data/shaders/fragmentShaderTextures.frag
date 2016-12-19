#version 330 core

in vec2 ex_TexCoords;
in vec3 ex_Normal;
in vec3 fLight;
in vec3 fEye;

out vec4 out_Color;
uniform sampler2D Texture;
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform float Shininess;

void main(void)
{
	vec3 N = normalize(ex_Normal);
    vec3 E = normalize(fEye);
    vec3 L = normalize(fLight);
    vec3 H = normalize(L + E);

	vec4 ambient = AmbientProduct;

    float Kd = max(dot(L, N), 0.0);
    vec4 diffuse = Kd * DiffuseProduct;

    float Ks = pow(max(dot(N, H), 0.0), 100.0);
    vec4 specular = Ks * SpecularProduct;

    if(dot(L,N) < 0.0) 
        specular = vec4(0.0, 0.0, 0.0, 1.0); 

    out_Color = texture(Texture, ex_TexCoords) * (ambient + diffuse) + specular;
}
