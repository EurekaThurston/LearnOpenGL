#version 330 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 posWS; 
    vec3 nDirWS;
    vec2 UV;
} VertexOutput;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

uniform bool inverse_normals;

void main() {
    gl_Position = P * V * M * vec4(aPos, 1.0);
    vec3 normal = inverse_normals ? -aNormal : aNormal;
    mat3 M_normal = transpose(inverse(mat3(M)));
    VertexOutput.posWS = vec3(M * vec4(aPos, 1.0));
    VertexOutput.nDirWS = M_normal * normal;
    VertexOutput.UV = aTexCoords;
}
