#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec3 nDirWS;
out vec2 UV;


void main() {
    gl_Position = P * V * M * vec4(aPos, 1.0);
    mat3 M_normal = mat3(transpose(inverse(M)));
    nDirWS = M_normal * aNormal;
    UV = aTexCoord;
}