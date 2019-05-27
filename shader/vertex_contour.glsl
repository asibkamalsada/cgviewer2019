#version 330 core

in vec3 position, normal;

uniform mat4 modelMatrix, viewMatrix, projectionMatrix;

void main(void)
{
    gl_Position = projectionMatrix * viewMatrix * (0.03 * normalize(modelMatrix * vec4(normal, 0.0)) + modelMatrix * vec4(position , 1.0));
}
