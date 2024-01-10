#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 TexCoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;


void main() {
    gl_Position = P * V * M * vec4(aPos, 1.0f);
}