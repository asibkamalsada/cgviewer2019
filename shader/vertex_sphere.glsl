#version 330 core

in vec3 position;

out vec3 hitPixel;

out vec3 rotatedPosition;

uniform mat4 viewMatrix, projectionMatrix;
uniform mat3 rotationMatrix;
uniform vec3 center;

uniform float radius;

void main(void)
{
    /*
    vec4 billBoard = viewMatrix * vec4(0.0, 0.0, 0.0, 1.0) + vec4((position).xy, 0.0, 1.0);
    hitPixel = (billBoard).xyz;
    gl_Position = projectionMatrix * billBoard;
    */



    gl_Position = projectionMatrix * viewMatrix * vec4(rotationMatrix * position, 1.0);
    //gl_Position = projectionMatrix * mat4(rotationMatrix) * viewMatrix * vec4( position, 1.0);
}
