#version 330 core

in vec3 position;

out vec3 fragmentCameraSpace;
out vec3 movedCenter;

uniform int frame;
uniform float movementFactor;
uniform mat4 viewMatrix, projectionMatrix;
uniform vec3 center;
uniform vec3 moveStep;


void main(void)
{

    movedCenter = center + float(frame) * moveStep;
    //movedCenter = center + movementFactor * moveStep;

    // source: https://wiki.delphigl.com/index.php/Sph%C3%A4risches_Billboard
    // this seems to be the same: http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/billboards/#solution-2--the-3d-way


    vec3 up = vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
    vec3 right = vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);

    /*
    // works perfectly fine, but isn't easy to read

    vec4 fragmentWorldSpace = vec4(position.x * right.x + position.y * up.x,
                                   position.x * right.y + position.y * up.y,
                                   position.x * right.z + position.y * up.z,
                                   1.0);
    */

    // this one is good too but there has to be something more elegant

    vec4 fragmentCameraSpaceAlpha = viewMatrix * vec4(movedCenter + (position.x * right + position.y * up), 1.0);
    gl_Position = projectionMatrix * fragmentCameraSpaceAlpha;

    fragmentCameraSpace = fragmentCameraSpaceAlpha.xyz;

}
