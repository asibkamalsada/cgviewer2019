#version 330 core

in vec3 position;

out vec4 fragmentCameraSpaceAlpha;
out vec3 fragmentWorldSpace;

uniform mat4 viewMatrix, projectionMatrix, rotationMatrix;
uniform vec3 color;
uniform float radius;
uniform vec3 center;
uniform vec3 camPos;
uniform vec3 normal;
uniform vec3 up;
uniform vec3 right;


void main(void)
{
    /*
    vec4 billBoard = viewMatrix * vec4(0.0, 0.0, 0.0, 1.0) + vec4((position).xy, 0.0, 1.0);
    hitPixel = (billBoard).xyz;
    gl_Position = projectionMatrix * billBoard;
    */

    /*
    vec3 centerToCam = camPos - center;
    vec3 centerToCamXZPlaneNormalized = normalize(vec3(centerToCam.x , 0, centerToCam.z));
    float angleCosine = dot(normal, centerToCamXZPlaneNormalized);
    vec3 upAux = cross(normal, centerToCamXZPlaneNormalized);
*/
/*
    vec4 fragmentWorldSpaceAlpha = rotationMatrix * vec4(position, 1.0);
    vec4 fragmentCameraSpaceAlpha =  viewMatrix * fragmentWorldSpaceAlpha;
    fragmentWorldSpace = fragmentWorldSpaceAlpha.xyz;
    fragmentCameraSpace = fragmentCameraSpaceAlpha.xyz;

    gl_Position = projectionMatrix * fragmentCameraSpaceAlpha ;*/
    //gl_Position = projectionMatrix * mat4(rotationMatrix) * viewMatrix * vec4( position, 1.0);


    vec4 fragmentWorldSpace = vec4(position.x / radius * right.x + position.y / radius * up.x + center.x,
                                   position.x / radius * right.y + position.y / radius * up.y + center.y,
                                   position.x / radius * right.z + position.y / radius * up.z + center.x,
                                   1.0);
    fragmentCameraSpaceAlpha = viewMatrix * fragmentWorldSpace;
    gl_Position = projectionMatrix * viewMatrix * fragmentWorldSpace;



}
