#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} GeometryInput[];

const float magnitude = 0.2;

uniform mat4 P;

void GenerateLine(int index) {
    gl_Position = P * gl_in[index].gl_Position;     // 起点
    EmitVertex();

    gl_Position = P * (gl_in[index].gl_Position + vec4(GeometryInput[index].normal, 0.0) * magnitude);
    EmitVertex();
    EndPrimitive();
}


void main() {    
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}