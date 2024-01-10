#version 330 core
out vec4 FragColor;

uniform vec3 baseCol;
uniform vec3 lightCol;

uniform float time;

void main() {
    FragColor = vec4(lightCol * baseCol, 1.0f);
}