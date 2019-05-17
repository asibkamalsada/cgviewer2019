#version 330 core

in vec3 position;

out vec3 fragmentCameraSpace;
out vec3 fragmentWorldSpace;

uniform mat4 viewMatrix, projectionMatrix, rotationMatrix;
uniform vec3 up;
uniform vec3 right;


void main(void)
{
    /*
      // this one doesnt work but should
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
//----------------------------------------------------------------------------------------------
    /*
    // works perfectly fine, but isn't easy to read

    vec4 fragmentWorldSpace = vec4(position.x * right.x + position.y * up.x,
                                   position.x * right.y + position.y * up.y,
                                   position.x * right.z + position.y * up.z,
                                   1.0);
    */

    // this one is good too but there has to be something more elegant
    //vec4 fragmentWorldSpace = vec4((position.x * right + position.y * up), 1.0);

    vec4 fragmentWorldSpaceAlpha = vec4((position.x * right + position.y * up), 1.0);
    vec4 fragmentCameraSpaceAlpha = viewMatrix * fragmentWorldSpaceAlpha;
    gl_Position = projectionMatrix * viewMatrix * fragmentWorldSpaceAlpha;

    fragmentWorldSpace = fragmentWorldSpaceAlpha.xyz;
    fragmentCameraSpace = fragmentCameraSpaceAlpha.xyz;
}
