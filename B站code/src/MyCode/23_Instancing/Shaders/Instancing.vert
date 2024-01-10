#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 nDirWS;

uniform vec3 offsets[100];
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main() {
    vec3 offsets = offsets[gl_InstanceID];
    gl_Position = P * V * M * vec4(aPos + offsets, 1.0);
    nDirWS = aNormal;
}