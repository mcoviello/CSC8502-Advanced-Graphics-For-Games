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
    if(diffuse.a == -1){
        fragColour.rgb = diffuse.xyz;
        fragColour.a = 1;
    } else {
    vec3 light = texture(diffuseLight, IN.texCoord).xyz;
    vec3 specular = texture(specularLight, IN.texCoord).xyz;

    fragColour.rgb = diffuse.xyz * 0.1; //ambient
    fragColour.rgb += diffuse.xyz * light;
    fragColour.rgb += specular;
    fragColour.a = 1;
    }
}