#version 330 core
out vec4 FragColor;

in vec3 nDirWS;
in vec2 UV;

uniform sampler2D texture_diffuse1;

void main() {
    float nDotL = max(dot(normalize(nDirWS), vec3(0.5, 0.5, 0.5)), 0.0);
    FragColor = texture(texture_diffuse1, UV) * nDotL * 2.0;
}