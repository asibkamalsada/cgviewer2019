#version 330 core

in vec3 worldSpacePosition;

uniform float radius;
uniform float distortionFactor;
uniform int frame;

uniform mat4 viewMatrix, projectionMatrix;

uniform vec3 cameraPosition;
uniform vec3 center;

uniform sampler2D sampler;

out vec4 finalColor;

void main(void)
{

    float factor = distortionFactor / 100;

    float tolerance = 0.1;

    float distanceFromCenter = length(worldSpacePosition - center);

    if ( distanceFromCenter > factor + radius - tolerance && distanceFromCenter < factor + radius )
    {
        //finalColor = vec4( 1.0 );
        finalColor = texture(sampler, vec2(sqrt(frame), sqrt(frame)));
/*    }
    else if ( distanceFromCenter > factor + radius )
    {
        finalColor = vec4( vec3( 0.0 ), 1.0 );*/
    } else {
        discard;
    }
}
