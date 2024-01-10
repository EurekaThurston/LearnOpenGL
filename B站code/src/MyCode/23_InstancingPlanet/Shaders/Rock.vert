#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in mat4 instanceMatrix;

uniform mat4 V;
uniform mat4 P;

out vec3 nDirWS;
out vec2 UV;

void main() {
    gl_Position = P * V * instanceMatrix * vec4(aPos, 1.0);
    mat3 M_normal = mat3(transpose(inverse(instanceMatrix)));
    nDirWS = M_normal * aNormal;
    UV = aTexCoord;
}