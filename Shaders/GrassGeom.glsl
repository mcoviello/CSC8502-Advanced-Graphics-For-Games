#version 330 core

uniform mat4 viewMatrix;
uniform mat4 projMatrix;

layout(triangles) in;
layout(triangle_strip , max_vertices = 3) out;
in  Vertex {
    vec4 colour;
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
    
void  main() {
    for(int i = 0; i < gl_in.length(); ++i) {
        OUT.colour      = vec4(0.6,1,0.8,1);
        OUT.normal      = IN[i].normal;
        OUT.tangent      = IN[i].tangent;
        OUT.binormal      = IN[i].binormal;

        gl_Position = gl_in[i].gl_Position;
        gl_Position.x -= 0;
        //gl_Position.y += 50;
        gl_Position += vec4(IN[i].normal,0) * 50;
        gl_Position = (projMatrix * viewMatrix) * gl_Position;
        EmitVertex();
        
        gl_Position = gl_in[i].gl_Position;
        gl_Position += vec4(IN[i].tangent,0) * 5;
        gl_Position = (projMatrix * viewMatrix) * gl_Position;
        EmitVertex();

        gl_Position = gl_in[i].gl_Position;
        gl_Position -= vec4(IN[i].tangent,0) * 5;
        gl_Position = (projMatrix * viewMatrix) * gl_Position;
        EmitVertex();

        EndPrimitive ();
    }
}

