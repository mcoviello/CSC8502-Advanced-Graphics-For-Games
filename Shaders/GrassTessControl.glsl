#version 400 core
layout(vertices = 3) out; //num vertices in output patch
uniform int tessAmount;

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
} OUT[]; // Equal to the size of the layout vertex count

void main() {

    gl_TessLevelInner [0] = tessAmount;
    gl_TessLevelOuter [0] = tessAmount;
    gl_TessLevelOuter [1] = tessAmount;
    gl_TessLevelOuter [2] = tessAmount;

    OUT[gl_InvocationID].normal = IN[gl_InvocationID].normal;
    OUT[gl_InvocationID].tangent = IN[gl_InvocationID].tangent;
    OUT[gl_InvocationID].binormal = IN[gl_InvocationID].binormal;
    OUT[gl_InvocationID].grassMapHeight = IN[gl_InvocationID].grassMapHeight;

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}