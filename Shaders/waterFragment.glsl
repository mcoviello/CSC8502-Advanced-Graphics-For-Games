#version 330 core
uniform sampler2D tex0; //bump
uniform sampler2D tex1; //island heightmap
uniform float time;

in Vertex{
    vec2 texCoord;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    vec3 worldPos;
} IN;

float clampedinvLerp(float a, float b, float value){
    return clamp((value - a) / (b - a), 0, 1);
}

out vec4 fragColour[2];

void main (void) {
    mat3 TBN = mat3(normalize(IN.tangent),
                    normalize(IN.binormal),
                    normalize(IN.normal));


    //Offsets the wave movement by sin wave movement, to simulate 'waves'
    float fakeWindOffset = (sin(time/100000 + IN.worldPos.x/700) + sin(time/900 + IN.worldPos.x/500) * 0.2 + 2) / 5;
    vec3 normal = texture2D(tex0, ((IN.texCoord*12) + vec2(fakeWindOffset, 0) - vec2(time/10000, 0))).rgb*2.0 - 1.0;
    normal = normalize(TBN * normalize(normal));

    float height = texture2D(tex1, (IN.texCoord)).r;
    float clampedHeight = clampedinvLerp(0.15, 0.25, height);
    
    fragColour[0] = vec4(0,0.8 ,0.7 ,1) + vec4(0.3,0.2,0.3,0) * clampedHeight;
    fragColour[0].a = 0.5;
    fragColour[1] = vec4((normal.rgb * 0.5 + 0.5), 0.2);
}