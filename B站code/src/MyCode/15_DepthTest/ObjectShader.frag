#version 330 core
out vec4 FragColor;

in vec3 nDirWS;
in vec3 posWS;
in vec2 UV;

float near = 0.1;
float far = 100.0;

float linearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far)/(far + near - z * (far - near));
}

void main() {
    float depth = linearizeDepth(gl_FragCoord.z)/far;
    FragColor = vec4(vec3(depth), 1.0);
}