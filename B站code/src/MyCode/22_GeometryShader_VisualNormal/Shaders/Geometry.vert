#version 330 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 normal;
} VertexOutput;

uniform mat4 V; 
uniform mat4 M; 

void main() {
    gl_Position = V * M * vec4(aPos, 1.0);
    mat3 M_Normal = mat3(transpose(inverse(V * M)));
    VertexOutput.normal = vec3(vec4(M_Normal * aNormal, 0.0));
}