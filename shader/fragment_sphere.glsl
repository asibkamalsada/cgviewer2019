#version 330 core

in vec3 fragmentCameraSpace, fragmentWorldSpace;

uniform vec3 color;
uniform float radius;
uniform vec3 center;
uniform vec3 camPos;
uniform mat4 viewMatrix;

out vec4 finalColor;

void main(void)
{

        vec3 viewCenter = (viewMatrix * vec4(center, 1.0)).xyz;
        vec3 rayVector = normalize(fragmentCameraSpace - camPos);
        float t = 0;
        float b = dot(2 * rayVector, camPos - viewCenter);
        float c = pow(length(camPos - viewCenter), 2) - pow(radius, 2);
        float D = pow(b, 2) - 4*c;
        if( D > 0) {
                t = ((-b) + sqrt(D))/2;
                vec3 p = camPos + t * rayVector;
                vec3 normal = normalize(p - viewCenter);
                finalColor = vec4( dot(normal, normalize(p - camPos)) * color, 1.0 );
        } else {
                finalColor = vec4(0.0, 0.0, 0.0, 0.0);
        }


    // - p / 2 + sqrt(pow((p/2),2) - q)
    //

/*

    vec3 viewCenter = (viewMatrix * vec4(center, 1.0)).xyz;

    float p = 2 * dot(fragmentCameraSpace,  - viewCenter);
    float q = dot( - viewCenter,  - viewCenter) - radius * radius;

    float d = pow(p,2) - 4*q;

    if (d < 0){
        discard;
    } else {
        finalColor = vec4(color, 1.0);
    }

*/
}
