#version 330 core 
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal; 
layout (location = 2) in vec2 aTexCoords;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out VS_OUT {
    vec3 posVS;
    vec3 nDirVS;
    vec2 UV;    
} VertexOutput;

void main() {
    gl_Position = P * V * M * vec4(aPos, 1.0);
    mat3 M_normal = transpose(inverse(mat3(V * M)));
    VertexOutput.posVS = vec3(V * M * vec4(aPos, 1.0));
    VertexOutput.nDirVS = M_normal * aNormal;
    VertexOutput.UV = aTexCoords;
}

