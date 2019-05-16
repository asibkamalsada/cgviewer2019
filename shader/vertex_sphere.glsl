#version 330 core

in vec3 position;

out vec3 fragmentCameraSpace;
out vec3 fragmentWorldSpace;

uniform mat4 viewMatrix, projectionMatrix, rotationMatrix;

void main(void)
{
    /*
    vec4 billBoard = viewMatrix * vec4(0.0, 0.0, 0.0, 1.0) + vec4((position).xy, 0.0, 1.0);
    hitPixel = (billBoard).xyz;
    gl_Position = projectionMatrix * billBoard;
    */
    vec4 fragmentWorldSpaceAlpha = rotationMatrix * vec4(position, 1.0);
    vec4 fragmentCameraSpaceAlpha = viewMatrix * fragmentWorldSpaceAlpha;
    fragmentWorldSpace = fragmentWorldSpaceAlpha.xyz;
    fragmentCameraSpace = fragmentCameraSpaceAlpha.xyz;

    gl_Position = projectionMatrix * fragmentCameraSpaceAlpha ;
    //gl_Position = projectionMatrix * mat4(rotationMatrix) * viewMatrix * vec4( position, 1.0);
}
