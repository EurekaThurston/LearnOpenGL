#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 color;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

uniform vec3 baseCol;
uniform vec3 lightCol;
uniform vec3 lightPos;
uniform vec3 camPos;

void main() {
    gl_Position = P * V * M * vec4(aPos, 1.0);
    vec3 posWS = (M * vec4(aPos, 1.0)).xyz;

    // Ambient
    float ambientInt = 0.1;
    vec3 ambient = ambientInt * baseCol;

    // Diffuse
    vec3 nDirWS = normalize(mat3(transpose(inverse(M))) * aNormal);
    vec3 lDirWS = normalize(lightPos - posWS);
    float diff = max(dot(nDirWS, lDirWS), 0.0);
    vec3 diffuse = diff * baseCol;

    // Specular
    float specInt = 0.5;
    vec3 vDirWS = normalize(camPos - posWS);
    vec3 lrDirWS = reflect(lDirWS, nDirWS);
    float spec = pow(max(dot(vDirWS, lrDirWS), 0.0), 64.0);
    vec3 specular = spec * specInt * baseCol;

    // Final Output
    color = ambient + diffuse + specular;
}