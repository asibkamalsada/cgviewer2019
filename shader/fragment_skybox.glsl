#version 330 core

// position of fragment in world coordinates
in vec3 fragmentPosition;

uniform samplerCube textureCube;


out vec4 finalColor;

void main(void)
{
    finalColor = texture(textureCube, fragmentPosition);
}
/*
in vec2 skyTexCoor;
uniform sampler2D texture;
out vec4 finalcolor;

void main(void)
{
        vec4 textureColor = texture(texture, skyTexCoor);

        finalcolor = textureColor;
}
*/
