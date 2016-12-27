#version 330 core

in vec2 ex_TexCoords;
in vec3 ex_Normal;
in vec3 fLight;
in vec3 fEye;
in vec3 vertPos;
in vec4 aProduct, dProduct, sProduct;

out vec3 out_Color;
uniform sampler2D Texture;

const vec3 lightPos = vec3(1.0,20.0,20.0);
const vec3 ambientColor = vec3(0.0, 0.0, 0.0);
const vec3 diffuseColor = vec3(1.0, 1.0, 1.0);
const vec3 specColor = vec3(1.0, 1.0, 1.0);
const float shininess = 20.0;
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
	  out_Color = texture(Texture, ex_TexCoords).xyz * colorGammaCorrected;
}
