#version 330 core

in vec3 fragmentCameraSpace;
in vec3 fragmentWorldSpace;

uniform vec3 color;
uniform float radius;
uniform vec3 center;

uniform mat4 viewMatrix, projectionMatrix;

out vec4 finalColor;

/*
float linearDepth(in float zw)
{
    return projectionMatrix[2][2] / (zw - projectionMatrix[2][3]);
}

float nonLinearDepth(in float input){
    return projectionMatrix[2][2] / input + projectionMatrix[2][3];
}
*/

void main(void)
{

    vec3 centerCameraSpace = (viewMatrix * vec4(center, 1.0)).xyz;


    // source: http://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection/
    // difference to source:
    // A: ray source, B: ray direction, t: ray factor,
    // P: point on sphere, C: center of sphere

    vec3 rayDirection = normalize(fragmentCameraSpace);

    float a = dot(rayDirection, rayDirection); // is very close to 1 since the direction is normalized
    float b = 2 * dot(rayDirection, - centerCameraSpace);
    float c = dot(centerCameraSpace, centerCameraSpace) - radius *radius;
    float d = b*b - 4 * a * c;

    if(d > - 0.0000001){
        float t1 = (-b + sqrt(d)) / (2 * a);
        float t2 = (-b - sqrt(d)) / (2 * a);
        float t = t1 < 0 ? (t2 < 0 ? 0 : t2) : (t2 < 0 ? t1 : min(t1, t2));

        vec3 pointOnSphereCameraSpace = t * rayDirection;

        // transform to Clip Space (this is the position the vertex shader normally puts out
        vec4 pointOnSphereClipSpace = projectionMatrix * vec4(pointOnSphereCameraSpace, 1.0);

        //// https://www.khronos.org/opengl/wiki/Vertex_Post-Processing

        // normalized device coordinates as a pre step where
        float zNDC = pointOnSphereClipSpace.z / pointOnSphereClipSpace.w;

        // gl_FragCoord is normally in window / screen space
        float zWindowSpace = (gl_DepthRange.far - gl_DepthRange.near) / 2 * zNDC
                                + (gl_DepthRange.far + gl_DepthRange.near) / 2;

        ////

        gl_FragDepth = zWindowSpace;


        finalColor = vec4(dot(-rayDirection, normalize(pointOnSphereCameraSpace - centerCameraSpace)) * color, //float*vec3
                          1.0);

        /* some funny testing
        if (gl_FragCoord.z < 0.001){
            finalColor = vec4(1.0);
        } else if (gl_FragCoord.z < 0.5) {
            finalColor = vec4(0.0, 1.0, 0.0, 1.0);
        } else if (gl_FragCoord.z < 0.99){
            finalColor = vec4(1.0, 0.0, 0.0, 1.0);
        } else {
            finalColor = vec4(0.0, 0.0, 1.0, 1.0);
        }

        //gl_FragDepth = 0.5;
        */

    } else {
        discard;
    }
}
