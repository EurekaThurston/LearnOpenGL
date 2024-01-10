#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform float time;
uniform sampler2D customTexture1;
uniform sampler2D customTexture2;

void main() {
    vec4 Tex1 = texture(customTexture1, TexCoord);
    vec4 Tex2 = texture(customTexture2, TexCoord);
    FragColor = mix(Tex1, Tex2, time);
}