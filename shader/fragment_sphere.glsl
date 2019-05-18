#version 330 core

in vec3 fragmentCameraSpace;
in vec3 fragmentWorldSpace;

uniform vec3 color;
uniform float radius;
uniform vec3 center;

uniform mat4 viewMatrix, projectionMatrix;

out vec4 finalColor;

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

    if(d > - 0.0001){
        float t1 = (-b + sqrt(d)) / (2 * a);
        float t2 = (-b - sqrt(d)) / (2 * a);
        float t = t1 < 0 ? (t2 < 0 ? 0 : t2) : (t2 < 0 ? t1 : min(t1, t2));

        vec3 pointOnSphereCameraSpace = t * rayDirection;



        // der neue depth wert berechnet sich aus dem Verhältnis vom billboard.z (fragmentCameraSpace.z)
        // zu spherePoint.z (pointOnSphereCameraSpace.z), also um wie viel weiter es nach vorn gerutscht ist, so viel
        // weiter muss der Wert von gl_FragDepth bzw. gl_FragCoord.z (äquivalente Werte) auch nach vorn rücken
        // jedoch ist gl_FragDepth ein output und gl_FragCoord ein input (not sure about that tho)
        gl_FragDepth = pointOnSphereCameraSpace.z / fragmentCameraSpace.z * gl_FragCoord.z;
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
