#version 330 core

in vec3 fragmentPosition;

uniform samplerCube textureCube;

out vec4 finalColor;

void main(void)
{

        //finalColor = vec4(1.0, 0.5, 0.5, 1.0);
        finalColor = vec4(0.25, 0.65, 1.0, 1.0);


       //finalColor = texture(textureCube, fragmentPosition);

}
