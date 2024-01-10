#version 330 core
out vec4 FragColor;

in vec2 UV;

uniform sampler2D screenTexture;

void main()
{ 
    FragColor = texture(screenTexture, UV);
}