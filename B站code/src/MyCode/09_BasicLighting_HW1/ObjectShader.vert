#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 nDir;
out vec3 posWS; 

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;


void main() {
    gl_Position = P * V * M * vec4(aPos, 1.0f);
    posWS = (M * vec4(aPos, 1.0f)).xyz;
    nDir = mat3(transpose(inverse(M))) * aNormal;
}