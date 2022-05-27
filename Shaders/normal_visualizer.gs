#version 330 core
layout (points) in;
layout (line_strip, max_vertices = 2) out;

uniform float normal_length;

in VS_OUT {
    vec4 normal;
} gs_in[];

void main() {    
    gl_Position = gl_in[0].gl_Position; 
    EmitVertex();
    gl_Position = gl_in[0].gl_Position + gs_in[0].normal * normal_length;
    EmitVertex();
    EndPrimitive();
}
