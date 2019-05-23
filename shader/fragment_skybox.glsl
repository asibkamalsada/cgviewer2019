#version 330 core

// position of fragment in world coordinates
in vec3 fragmentPosition;

uniform samplerCube textureCube;


out vec4 finalColor;

void main(void)
{
    finalColor = texture(textureCube, fragmentPosition);
}
