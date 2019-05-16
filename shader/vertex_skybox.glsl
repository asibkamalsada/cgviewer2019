#version 330 core

in vec3 position;

out vec3 fragmentPosition;

uniform mat4 viewMatrix, projectionMatrix;

uniform vec3 cameraPosition;

void main(void)
{

    fragmentPosition = position;// + cameraPosition;

    gl_Position = projectionMatrix * viewMatrix * vec4(position
                                                       + cameraPosition
                                                       , 1.0);
}
/*

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform vec3 cameraPositionSky;

out vec2 skyTexCoor;

void main(void)
{
        skyTexCoor = texCoords;
        gl_Position = projectionMatrix * viewMatrix * vec4(position + cameraPositionSky, 1.0);
}
*/
