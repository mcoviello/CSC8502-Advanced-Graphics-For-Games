#version 330 core
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;
in vec4 colour;
in vec3 normal;
in vec4 tangent;

out Vertex{
    vec4 colour;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
} OUT;

void main(void){
    OUT.colour = colour;

    //Calculate the inverse of the model matrix, while removing translational data
    mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    vec3 wNormal = normalize(normalMatrix * normalize(normal));
    vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));

    OUT.normal = wNormal;
    OUT.tangent = tangent.xyz;
    OUT.binormal = cross(wTangent, wNormal) * tangent.w;

    vec4 worldPos = (modelMatrix * vec4(position, 1));
    gl_Position = (projMatrix * viewMatrix) * worldPos;
}