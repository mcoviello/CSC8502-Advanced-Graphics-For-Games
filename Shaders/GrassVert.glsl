#version 330 core
uniform mat4 modelMatrix;
uniform sampler2D grassMap;
uniform int mapScale;

in vec3 position;
in vec3 normal;
in vec4 tangent;
in vec2 texCoord;

out Vertex{
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    float grassMapHeight;
} OUT;

void main(void){
    OUT.grassMapHeight = texture2D(grassMap, texCoord / mapScale).r;

    OUT.normal = normal;
    OUT.tangent = tangent.xyz;
    OUT.binormal = cross(normal, tangent.xyz) * tangent.w;
    
    vec4 worldPos = (modelMatrix * vec4(position, 1));
    gl_Position =  worldPos;
}