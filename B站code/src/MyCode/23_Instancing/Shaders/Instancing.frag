#version 330 core
out vec4 FragColor;

in vec3 nDirWS;

void main() {
    FragColor = vec4(normalize(nDirWS), 1.0);
}