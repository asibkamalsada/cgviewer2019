#version 330 core

in vec3 hitPixel;

uniform vec3 color;
uniform float radius;
uniform vec3 center;
uniform vec3 camPos;
uniform mat4 viewMatrix;

out vec4 finalcolor;

void main(void)
{
    /*
        vec3 viewCenter = (viewMatrix * vec4(center, 1.0)).xyz;
        vec3 rayVector = normalize(hitPixel - camPos);
        float t = 0;
        float b = dot(2 * rayVector, camPos - viewCenter);
        float c = pow(length(camPos - viewCenter), 2) - pow(radius, 2);
        float D = pow(b, 2) - 4*c;
        if( D > 0) {
                t = ((-b) + sqrt(D))/2;
                vec3 p = camPos + t * rayVector;
                vec3 normal = normalize(p - viewCenter);
                finalcolor = vec4( dot(normal, normalize(p - camPos)) * color, 1.0 );
                finalcolor.a = 1.0;
        } else {
                finalcolor = vec4(0.0, 0.0, 0.0, 0.0);
        }

    */
    finalcolor = vec4(color, 1.0);
}
