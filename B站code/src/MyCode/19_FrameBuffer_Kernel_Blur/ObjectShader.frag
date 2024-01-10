#version 330 core
out vec4 FragColor;

in vec3 nDirWS;
in vec3 posWS;
in vec3 posVS;
in vec2 UV;

void main() {
    FragColor = vec4(vec3(0.8), 1.0);
}