#version 330 core
uniform mat4 modelMatrix;

in vec3 position;
in vec3 normal;
in vec4 tangent;

out Vertex{
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
} OUT;

void main(void){

    OUT.normal = normal;
    OUT.tangent = tangent.xyz;
    OUT.binormal = cross(normal, tangent.xyz) * tangent.w;
    
    vec4 worldPos = (modelMatrix * vec4(position, 1));
    gl_Position =  worldPos;
}