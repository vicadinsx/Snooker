#version 330 core

in vec2 ex_TexCoords;
in vec3 ex_Normal;

in vec3 vertPos;

in vec3 ambientColor, diffuseColor, specColor,lightPos;

out vec3 out_Color;
uniform sampler2D Texture;
uniform bool isSepia;

const float shininess = 100.0;
const float screenGamma = 2.2; // Assume the monitor is calibrated to the sRGB color space
const int mode = 0;

void main() {

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
       
    // this is phong (for comparison)
    if(mode == 2) {
      vec3 reflectDir = reflect(-lightDir, normal);
      specAngle = max(dot(reflectDir, viewDir), 0.0);
      // note that the exponent is different here
      specular = pow(specAngle, shininess/4.0);
    }
  }

  vec3 colorLinear = ambientColor +
                     lambertian * diffuseColor +
                     specular * specColor;

  // apply gamma correction (assume ambientColor, diffuseColor and specColor
  // have been linearized, i.e. have no gamma correction in them)
	vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0/screenGamma));
  

    // use the gamma corrected color in the fragment	
	vec3 color = texture(Texture, ex_TexCoords).xyz * colorGammaCorrected;
	if(isSepia)
	{
		float gray = dot(color, vec3(0.299, 0.587, 0.114));
		out_Color  = gray * vec3(1.2, 1.0, 0.8);
	}
	else out_Color = color;

}
