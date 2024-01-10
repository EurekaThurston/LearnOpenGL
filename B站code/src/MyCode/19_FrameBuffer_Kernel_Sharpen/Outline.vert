#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

out vec3 nDirWS;
out vec3 posWS; 
out vec3 posVS;
out vec2 UV;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat3 M_normal;
uniform float scale;

float near = 0.1;
float far = 100.0;

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far)/(far + near - z * (far - near));
}

void main() {
    gl_Position = P * V * M * vec4(aPos, 1.0);
    posWS = (M * vec4(aPos, 1.0f)).xyz;
    posVS = aPos;
    nDirWS = normalize(M_normal * aNormal);
    UV = aUV;
}