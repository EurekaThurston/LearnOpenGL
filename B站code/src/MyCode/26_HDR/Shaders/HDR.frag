#version 330 core
out vec4 FragColor;

in vec2 UV;

uniform sampler2D hdrBuffer;
uniform float exposure;

void main() {
    vec3 hdrCol = texture(hdrBuffer, UV).rgb;
    vec3 result = vec3(1.0) - exp(-hdrCol * exposure);
    result = pow(result, vec3(1.0/2.2));
    FragColor = vec4(result, 1.0);
}