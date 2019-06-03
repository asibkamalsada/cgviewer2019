#version 330 core


in vec3 fragmentNormalNormalized, worldSpacePosition;

out vec4 finalColor;

uniform vec3 cameraPosition; // in the world space
uniform vec3 matAmbient, matDiffuse, matSpecular;
uniform float shine, alpha;
uniform vec3 lightsAmbientArray[3], lightsDiffuseArray[3], lightsSpecularArray[3], lightsPositionArray[3];
uniform int lightsSize;

uniform sampler2D texture;
in vec2 textureUV;


void main(void)
{
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);

    for (int i = 0; i < lightsSize; i++ ){
        vec3 fragmentToLightNormalized = normalize(lightsPositionArray[i] -  worldSpacePosition);
        vec3 lightToFragmentNormalized = -fragmentToLightNormalized;

        ambient += matAmbient * lightsAmbientArray[i];

        float diffuseCelFactor, diffusePhongFactor;

        diffusePhongFactor = max(0.0, dot(fragmentNormalNormalized, fragmentToLightNormalized));

        if (diffusePhongFactor <= 0.33) {
            diffuseCelFactor = 0.0;
        } else if (diffusePhongFactor <= 0.8) {
            diffuseCelFactor = 0.5;
        } else {
            diffuseCelFactor = 1.0;
        }

        diffuse += matDiffuse * lightsDiffuseArray[i] * diffuseCelFactor;

        float specularCelFactor, specularPhongFactor;

        specularPhongFactor = pow(max(0.0,
                                      dot(normalize(cameraPosition - worldSpacePosition), // from fragment to viewer
                                          normalize(reflect(lightToFragmentNormalized, fragmentNormalNormalized))))
                                  , shine);

        if (specularPhongFactor <= 0.3) {
            specularCelFactor = 0.0;
        } else if (specularPhongFactor <= 0.7){
            specularCelFactor = 0.2;
//        } else if (specularPhongFactor <= 0.8){
//        specularCelFactor = 0.3;
        } else if (specularPhongFactor <= 0.9){
            specularCelFactor = 0.5;
//        } else if (specularPhongFactor <= 0.97){
//            specularCelFactor = 0.7;
        } else {
            specularCelFactor = 1.0;
        }

        specular += matSpecular * lightsSpecularArray[i] * specularCelFactor;
    }

    //float edgeDetection = (fragmentNormalNormalized.y != 0 && fragmentNormalNormalized.x != 0 && dot(normalize(cameraPosition - worldSpacePosition), fragmentNormalNormalized) > 0.4) ? 1 : 0;
    float edgeDetection = 1;

    // changed texture2D to texture, should work on latest OpenGL now 
    vec3 textureColor = texture(texture, textureUV).rgb;

    vec3 lightedColor = ambient + diffuse + specular;
/*
    float luminance = dot(lightedColor,vec3(0.2126, 0.7152, 0.0722));
    float gradient = fwidth(luminance );
    edgeDetection = gradient < 0.00001 ? 1 : 0;
*/
    //edgeDetection = 1;

    lightedColor = lightedColor * edgeDetection;

    if (textureUV.x < 0 || textureUV.y < 0 ) {
            //finalColor = vec4(lightedColor.z, lightedColor.y, lightedColor.x, alpha);
            finalColor = vec4(lightedColor, alpha);
       }
       else {
            finalColor = vec4(textureColor * lightedColor, alpha);
    }

}
