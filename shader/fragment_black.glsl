#version 330 core

in vec3 fragmentPosition;

out vec4 finalColor;

void main(void)
{

       //finalColor = vec4(1.0, 0.5, 0.5, 1.0);
       finalColor = vec4(0, 0, 0, 1);


       //finalColor = texture(textureCube, fragmentPosition);

}
