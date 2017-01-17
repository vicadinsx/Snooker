#version 330 core

uniform sampler2D Texture;
uniform vec4 DarkWoodColor = vec4( 0.44, 0.24, 0.06, 1.0 );
uniform vec4 LightWoodColor = vec4( 0.64, 0.39, 0.11, 1.0 );
uniform mat4 Slice;

in vec2 ex_TexCoords;
in vec3 ex_Normal;

in vec3 vertPos;

in vec3 ambientColor, diffuseColor, specColor,lightPos;

out vec4 FragColor;

const float shininess = 100.0;
const float screenGamma = 2.2; // Assume the monitor is calibrated to the sRGB color space

void main()
{


	vec3 normal = normalize(ex_Normal);
  	vec3 lightDir = normalize(lightPos - vertPos);

  	float lambertian = max(dot(lightDir,normal), 0.0);
  	float specular = 0.0;

  	if(lambertian > 0.0) {
	    vec3 viewDir = normalize(-vertPos);
	    // this is blinn phong
	    vec3 halfDir = normalize(lightDir + viewDir);
	    float specAngle = max(dot(halfDir, normal), 0.0);
	    specular = pow(specAngle, shininess);
	}

  	vec3 colorLinear = ambientColor +
                     lambertian * diffuseColor +
                     specular * specColor;

  	// apply gamma correction (assume ambientColor, diffuseColor and specColor
  	// have been linearized, i.e. have no gamma correction in them)
	vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0/screenGamma));

    // use the gamma corrected color in the fragment	
	vec3 color_light = texture(Texture, ex_TexCoords).xyz * colorGammaCorrected;				

	vec2 tc = ex_TexCoords;
	if( tc.s > 0.5 ) tc.s = 1.0 - tc.s;
	vec4 cyl = Slice * vec4( tc, 0.0, 1.0 );

	// Transform the texture coordinates to define the
	// "slice" of the log.
	//vec4 cyl = Slice * vec4( ex_TexCoords.st, 0.0, 1.0 );

	// The distance from the log's y axis.
	float dist = length(cyl.xz);

	// Perturb the distance using the noise texture
	vec4 noise = texture(Texture, ex_TexCoords);
	dist += noise.b;

	// Determine the color as a mixture of the light and
	// dark wood colors.
	float t = 1.0 - abs( fract( dist ) * 2.0 - 1.0 );
	t = smoothstep( 0.2, 0.8, t );

	vec4 color = mix( DarkWoodColor, LightWoodColor, t );
	FragColor = vec4(color_light,1.0) * vec4( color.rgb , 1.0 );

}