#version 330 core

in Vertex {
    vec4 colour;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
} IN;

out vec4 fragColour;

void main(void) {
    fragColour = IN.colour;
    fragColour.a = 0;
}