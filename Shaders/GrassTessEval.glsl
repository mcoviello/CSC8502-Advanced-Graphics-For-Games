#version 400 core

layout(triangles, equal_spacing, cw, point_mode) in;

uniform mat4 modelMatrix;

in Vertex {
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    float grassMapHeight;
} IN[]; //Equal to size of the draw call vertex count

out Vertex {
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    float grassMapHeight;
} OUT; // Equal to the size of the layout vertex count

void main() {
    vec3 p0 = gl_TessCoord.x * gl_in [0]. gl_Position.xyz;
    vec3 p1 = gl_TessCoord.y * gl_in [1]. gl_Position.xyz;
    vec3 p2 = gl_TessCoord.z * gl_in [2]. gl_Position.xyz;

    vec3 combinedPos = p0 + p1 + p2;

    OUT.normal = IN[0].normal;
    OUT.tangent = IN[0].tangent;
    OUT.binormal = IN[0].binormal;
    OUT.grassMapHeight = IN[0].grassMapHeight;

    vec4 worldPos = modelMatrix * vec4(combinedPos , 1);
    gl_Position = worldPos;
    //gl_Position = projMatrix * viewMatrix * worldPos;
}