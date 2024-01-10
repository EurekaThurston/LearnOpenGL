#version 330 core
out vec4 FragColor;

in vec3 UV;

uniform samplerCube Cubemap;

void main() {
    FragColor = texture(Cubemap, UV);
}