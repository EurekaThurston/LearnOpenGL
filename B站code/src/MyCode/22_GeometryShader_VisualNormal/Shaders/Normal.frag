#version 330 core
out vec4 FragColor;

in vec3 nDirWS;

void main() {
    vec3 nDirWS = normalize(nDirWS);
    vec3 lDirWS = vec3(0.5);
    FragColor = vec4(vec3(max(dot(nDirWS, lDirWS), 0.0)), 1.0);
}