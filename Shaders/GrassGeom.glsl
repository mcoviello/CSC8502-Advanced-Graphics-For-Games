#version 330 core

uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform float time;

layout(triangles) in;
layout(triangle_strip , max_vertices = 5) out;
in  Vertex {
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
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

        vec4 tipColour = vec4(0.3, 1, 0.6, 1);
        vec4 baseColour = vec4(0.15, 1, 0.45, 1);

    for(int i = 0; i < gl_in.length(); ++i) {
        OUT.normal      = IN[i].normal;
        OUT.tangent      = IN[i].tangent;
        OUT.binormal      = IN[i].binormal;

        //Get random blade height
        float height = 50 + (((rand(gl_in[i].gl_Position.xz))) * 100);

        //Calculate a 'random' rotation for the grass based off its xz world position.
        //Create the grass geometry along this rotation axis
        float randomRot = rand(gl_in[i].gl_Position.xz);
        vec3 bladeAxis = normalize((IN[i].binormal * randomRot) + (IN[i].tangent * (1-randomRot)));

        //Calculate wind strength for blade of grass using two offset sin waves
        //Use X position to simulate "waves" of wind
        vec4 windStrength = vec4(-IN[i].tangent,0) * (((sin(time * 0.003 + (gl_in[i].gl_Position.x/300))+1)/2) + (sin(time * 0.0007)));

        // -- Draw Grass Blade --//
        //TODO: Make this dynamic, with an adjustable height (and segment count?)

        gl_Position = gl_in[i].gl_Position;
        gl_Position += vec4(bladeAxis,0) * 5; 
        gl_Position = (projMatrix * viewMatrix) * gl_Position;
        OUT.colour = baseColour;
        EmitVertex();
        
        gl_Position = gl_in[i].gl_Position;
        gl_Position -= vec4(bladeAxis,0) * 5;
        gl_Position = (projMatrix * viewMatrix) * gl_Position;
        EmitVertex();

        gl_Position = gl_in[i].gl_Position;
        gl_Position += vec4(bladeAxis,0) * 5;
        gl_Position += vec4(IN[i].normal,0) * height / 2;
        gl_Position += (windStrength* height / 70) * 3;
        gl_Position = (projMatrix * viewMatrix) * gl_Position;
        EmitVertex();

        gl_Position = gl_in[i].gl_Position;
        gl_Position -= vec4(bladeAxis,0) * 2;
        gl_Position += vec4(IN[i].normal,0) * height / 3;
        gl_Position += (windStrength* height / 70) * 5; 
        gl_Position = (projMatrix * viewMatrix) * gl_Position;
        EmitVertex();

        gl_Position = gl_in[i].gl_Position;
        gl_Position -= vec4(bladeAxis,0) * 7;
        gl_Position += vec4(IN[i].normal,0) * height;
        gl_Position += (windStrength * height / 70) * 25; 
        OUT.colour = tipColour;
        gl_Position = (projMatrix * viewMatrix) * gl_Position;
        EmitVertex();

        EndPrimitive ();
    }
}

