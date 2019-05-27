#version 330 core

in vec3 position;

uniform mat4 modelMatrix, viewMatrix, projectionMatrix;

void main(void)
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position , 1.0);
}
