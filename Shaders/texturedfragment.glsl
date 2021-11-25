#version 330 core
uniform sampler2D tex0;

in Vertex{
    vec2 texCoord;
} IN;

out vec4 fragColour;
void main(void){
    fragColour = texture(tex0, IN.texCoord);
    fragColour.a = 1;
}