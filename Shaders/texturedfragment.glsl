#version 330 core
uniform sampler2D tex0;

in Vertex{
    vec2 texCoord;
} IN;

out vec4 fragColour[2];
void main(void){
    fragColour[0].rgb = texture(tex0, IN.texCoord).rgb;
    fragColour[0].a = 0;
    fragColour[1].rgb = vec3(1,1,1);
    fragColour[1].a = 0;
}