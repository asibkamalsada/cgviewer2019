#version 330 core

in vec3 fragmentCameraSpace;
in vec3 fragmentWorldSpace;

uniform int frame;
uniform vec3 moveStep;

uniform float radius;
in vec3 movedCenter;

uniform mat4 viewMatrix, projectionMatrix;

uniform vec3 cameraPosition;

uniform samplerCube textureCube;

out vec4 finalColor;

void main(void)
{

    vec3 movedCenterCameraSpace = (viewMatrix * vec4(movedCenter, 1.0)).xyz;


//// http://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection/
    // difference to source:
    // A: ray source, B: ray direction, t: ray factor,
    // P: point on sphere, C: movedCenter of sphere
    // sphere: dot(pointOnSphere - movedCenter, pointOnSphere - movedCenter) - r * r = 0
    // exiting sphere with refracted vector: pointOnSphere = pointOnSphereCameraSpace + enteringRefractedDirectionNormalized * t

    vec3 rayDirection = normalize(fragmentCameraSpace);
    vec3 rayOrigin = vec3(0.0); // vector is sent from camera in cameraSpace which is the coordinate systems origin


    float a = dot(rayDirection, rayDirection); // is very close to 1 since the direction is normalized
    float b = 2 * dot(rayDirection, rayOrigin - movedCenterCameraSpace);
    float c = dot( rayOrigin - movedCenterCameraSpace, rayOrigin - movedCenterCameraSpace) - radius *radius;
    float d = b * b - 4 * a * c;

    if(d > - 0.0000001){
        float t1 = (-b + sqrt(d)) / (2 * a);
        float t2 = (-b - sqrt(d)) / (2 * a);
        float t = t1 < 0 ? (t2 < 0 ? 0 : t2) : (t2 < 0 ? t1 : min(t1, t2));

        vec3 pointOnSphereCameraSpace = t * rayDirection;

        // transform to Clip Space (this is the position the vertex shader normally puts out
        vec4 pointOnSphereClipSpace = projectionMatrix * vec4(pointOnSphereCameraSpace, 1.0);

//// https://www.khronos.org/opengl/wiki/Vertex_Post-Processing

        // normalized device coordinates as a pre step
        float zNDC = pointOnSphereClipSpace.z / pointOnSphereClipSpace.w;

        // gl_FragCoord is normally in window / screen space
        float zWindowSpace = (gl_DepthRange.far - gl_DepthRange.near) / 2 * zNDC
                                + (gl_DepthRange.far + gl_DepthRange.near) / 2;

        gl_FragDepth = zWindowSpace;


//// https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel

        vec3 enteringPointOnSphereWorldSpace = (inverse(viewMatrix) * vec4( pointOnSphereCameraSpace, 1.0)).xyz;

        vec3 enterNormalNormalized = normalize(enteringPointOnSphereWorldSpace - movedCenter);
        vec3 cameraToEnterNormalized = normalize(enteringPointOnSphereWorldSpace - cameraPosition);

        float refractiveIndex = 1.5;
        vec3 enteringRefractedDirectionNormalized = refract(cameraToEnterNormalized,
          enterNormalNormalized, 1.0 / refractiveIndex);

        vec3 refractedRay = enteringRefractedDirectionNormalized;

        // exiting point calculated in worldSpace since it makes no sense to transform to cameraSpace and back

        // a = dot(enteringRefractedDirectionNormalized, enteringRefractedDirectionNormalized); is 1
        b = 2 * dot(enteringRefractedDirectionNormalized, enteringPointOnSphereWorldSpace - movedCenter);
        c = dot( enteringPointOnSphereWorldSpace - movedCenter, enteringPointOnSphereWorldSpace - movedCenter) - radius *radius;
        d = b * b - 4 /** a */ * c; // a is 1

        if(d > - 0.0000001){
            t1 = (-b + sqrt(d)) / (2 /* * a */);
            // t2 = (-b - sqrt(d)) / (2 * a); can be ignored
            //
            // <-------y  =
            //      =    ^_  =
            //     =       \_ =
            //     =         \x<------------   incoming ray
            //      =        =
            //         =  =
            //
            //  in this exaggerated picture, pointOnCircle1 = y = x + t1 * refractedRay(inside the circle) and
            //  t1 is obviously bigger than 0
            //  we are calculating both t which would make a point on the sphere (in this case circle) so
            //  pointOnCircle2 = x = x + t2 * refractedRay, with t2 obviously being 0
            //

            t = t1;

            vec3 exitingPointOnSphereWorldSpace = enteringPointOnSphereWorldSpace + t * enteringRefractedDirectionNormalized;

            vec3 exitNormalNormalized = normalize(exitingPointOnSphereWorldSpace - movedCenter);
            vec3 cameraToExitNormalized = normalize(exitingPointOnSphereWorldSpace - cameraPosition);
            vec3 exitingRefractedDirectionNormalized = refract(cameraToExitNormalized, exitNormalNormalized, refractiveIndex / 1.0);

            refractedRay = exitingRefractedDirectionNormalized;

        }

        finalColor = texture(textureCube, refractedRay);


    } else {
        discard;
    }

}
