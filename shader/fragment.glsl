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
        diffuse += matDiffuse * lightsDiffuseArray[i] * max(0.0, dot(fragmentNormalNormalized, fragmentToLightNormalized));
        specular += matSpecular * lightsSpecularArray[i] *
                pow(max(0.0,
                        dot(normalize(cameraPosition - worldSpacePosition), // from fragment to viewer
                            normalize(reflect(lightToFragmentNormalized, fragmentNormalNormalized))))
                    , shine);
    }


    // changed texture2D to texture, should work on latest OpenGL now 
    vec3 textureColor = texture2D(texture, textureUV).rgb;

    vec3 lightedColor = ambient + diffuse + specular;

    if (textureUV.x < 0 || textureUV.y < 0 ) {
            finalColor = vec4(lightedColor.z, lightedColor.y, lightedColor.x, alpha);
            //finalColor = vec4(lightedColor, alpha);
       }
       else {
            finalColor = vec4(textureColor * lightedColor, alpha);
    }

}
