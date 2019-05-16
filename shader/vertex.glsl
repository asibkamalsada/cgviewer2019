#version 330 core

in vec3 position, normal;
in vec2 texCoords;
//                                 FragPos
out vec3 fragmentNormalNormalized, worldSpacePosition;
out vec2 textureUV;


uniform mat4 normalMatrix, modelMatrix, viewMatrix, projectionMatrix;

void main(void)
{

    //fragmentNormalNormalized = normalize(normalMatrix * vec4(normal, 0.0)).xyz;

    fragmentNormalNormalized = normalize(modelMatrix * vec4(normal, 0.0)).xyz;

    worldSpacePosition = (modelMatrix * vec4(position, 1.0)).xyz;

    textureUV = texCoords;

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
}
