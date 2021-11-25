#version 330 core
uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;

in Vertex{
    vec2 texCoord;
} IN;

out vec4 fragColour;

void main(void){
    vec4 diffuse = texture(diffuseTex, IN.texCoord).rgba;
    vec3 light = texture(diffuseLight, IN.texCoord).xyz;
    vec3 specular = texture(specularLight, IN.texCoord).xyz;

    light.r = floor((light.r * 8) + 0.5) / 8;
    light.g = floor((light.g * 8) + 0.5) / 8;
    light.b = floor((light.b * 8) + 0.5) / 8;


    if(diffuse.a > 0.9){
        fragColour.rgb = diffuse.rgb;
        fragColour.a = diffuse.a;
    } else {
    fragColour.rgb = diffuse.rgb * 0.1; //ambient
    fragColour.rgb += diffuse.rgb * light;
    fragColour.rgb += specular;
    fragColour.a = 1;
    }
}