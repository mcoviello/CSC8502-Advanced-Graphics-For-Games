#version 330 core

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform float time;

layout(points) in;
layout(triangle_strip , max_vertices = 5) out;
in Vertex {
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    float grassMapHeight;
    } IN[];
    
out  Vertex {
    vec4 colour;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    } OUT;
    
// Pseudorandom noise function, by:
// Appas, from https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void  main() {

    vec4 tipColour = vec4(0.15, 1, 0.45, 1);
    vec4 baseColour = vec4(0.3, 1, 0.6, 1);
    if(IN[0].grassMapHeight > 0.0){
        OUT.normal      = IN[0].normal;
        OUT.tangent      = IN[0].tangent;
        OUT.binormal      = IN[0].binormal;

        //Get random blade height
        float height = 50 + (((rand(gl_in[0].gl_Position.xz))) * 100);
        height *= IN[0].grassMapHeight;

        //Calculate a 'random' rotation for the grass based off its xz world position.
        //Create the grass geometry along this rotation axis
        float randomRot = rand(gl_in[0].gl_Position.xz);
        vec3 bladeAxis = normalize((IN[0].binormal * randomRot) + (IN[0].tangent * (1-randomRot)));

        //Calculate wind strength for blade of grass using two offset sin waves
        //Use X position to simulate "waves" of wind
        vec4 windStrength = vec4(-IN[0].tangent,0) * (((sin(time * 0.003 + (gl_in[0].gl_Position.x/300))+1)/2) + (sin(time * 0.0007)));

        // -- Draw Grass Blade --//
        //TODO: Make this dynamic, with an adjustable segment count

        gl_Position = gl_in[0].gl_Position;
        gl_Position += vec4(bladeAxis,0) * 5; 
        gl_Position = (projMatrix * viewMatrix) * gl_Position;
        OUT.colour = baseColour;
        EmitVertex();
        
        gl_Position = gl_in[0].gl_Position;
        gl_Position -= vec4(bladeAxis,0) * 5;
        gl_Position = (projMatrix * viewMatrix) * gl_Position;
        EmitVertex();

        gl_Position = gl_in[0].gl_Position;
        gl_Position += vec4(bladeAxis,0) * 5;
        gl_Position += vec4(IN[0].normal,0) * height / 2;
        gl_Position += (windStrength* height / 70) * 3;
        gl_Position = (projMatrix * viewMatrix) * gl_Position;
        EmitVertex();

        gl_Position = gl_in[0].gl_Position;
        gl_Position -= vec4(bladeAxis,0) * 2;
        gl_Position += vec4(IN[0].normal,0) * height / 3;
        gl_Position += (windStrength* height / 70) * 5; 
        gl_Position = (projMatrix * viewMatrix) * gl_Position;
        EmitVertex();

        gl_Position = gl_in[0].gl_Position;
        gl_Position -= vec4(bladeAxis,0) * 7;
        gl_Position += vec4(IN[0].normal,0) * height;
        gl_Position += (windStrength * height / 70) * 25; 
        OUT.colour = tipColour;
        gl_Position = (projMatrix * viewMatrix) * gl_Position;
        EmitVertex();

        EndPrimitive ();
    }
}

