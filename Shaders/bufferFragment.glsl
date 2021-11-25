#version 330 core

/* Allows texture painting of three textures and three normals, with a seperate texture's rgb channels. */

uniform sampler2D tex0; //sand, mapped to red
uniform sampler2D tex1; //sandbump
uniform sampler2D tex2; //Rock, mapped to green
uniform sampler2D tex3; //rockbump
uniform sampler2D tex4; //grass, mapped to blue
uniform sampler2D tex5; //grassbump
uniform sampler2D tex6; //texture painting


in Vertex{
    vec4 colour;
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 worldPos;
} IN;

out vec4 fragColour[2];

void main(void){
    mat3 TBN = mat3(normalize(IN.tangent),
                    normalize(IN.binormal),
                    normalize(IN.normal));
    vec3 normal = texture2D(tex1, IN.texCoord/2).rgb*2.0 - 1.0;
    normal = normalize(TBN * normalize(normal));
    vec4 curTexCol = texture2D(tex6, IN.texCoord/32).rgba;

    fragColour[0].rgb = texture2D(tex0, IN.texCoord).rgb * curTexCol.r;
    fragColour[0].rgb += texture2D(tex2, IN.texCoord).rgb * curTexCol.g;
    fragColour[0].rgb += texture2D(tex4, IN.texCoord).rgb * curTexCol.b;
    fragColour[0].a = 0;

    fragColour[1].rgb = texture2D(tex1, IN.texCoord).rgb * curTexCol.r;
    fragColour[1].rgb += texture2D(tex3, IN.texCoord).rgb * curTexCol.g;
    fragColour[1].rgb += texture2D(tex5, IN.texCoord).rgb * curTexCol.b;
    fragColour[1].a = 1;

}