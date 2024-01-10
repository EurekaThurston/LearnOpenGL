#version 330 core
layout (location = 0) in vec3 aPos;

layout (std140) uniform Matrices {
    mat4 P;
    mat4 V;
};

uniform mat4 M;

void main() {
    gl_Position = P * V * M * vec4(aPos, 1.0);
}