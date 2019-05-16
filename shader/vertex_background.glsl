#version 330 core

in vec3 position;
//                                 FragPos
out vec3 fragmentPosition;

uniform mat4 viewMatrix, projectionMatrix;

uniform vec3 cameraPosition;

void main(void)
{

    fragmentPosition = position;
    //puts the camera in a box
    gl_Position = projectionMatrix * viewMatrix * vec4(position + cameraPosition, 1.0);
}
