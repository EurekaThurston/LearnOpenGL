#version 330 core
out vec4 FragColor;

in vec3 nDirWS;
in vec3 posWS;
in vec3 posVS;
in vec2 UV;

float near = 0.1;
float far = 100.0;

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far)/(far + near - z * (far - near));
}

void main() {
    FragColor = vec4(vec3(0.0), 1.0);
}