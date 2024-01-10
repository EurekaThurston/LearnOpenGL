#version 330 core
out vec4 FragColor;

in vec3 nDir;
in vec3 posWS;

uniform vec3 baseCol;
uniform vec3 lightCol;
uniform vec3 lightPos;
uniform vec3 camPos; 

void main() {
    // 环境光照
    float ambientInt = 0.1;
    vec3 ambient = ambientInt * lightCol;

    // 漫反射
    vec3 nDirWS = normalize(nDir);
    vec3 lightDir = normalize(lightPos - posWS);
    float diff = max(dot(nDirWS, lightDir), 0.0f);
    vec3 diffuse = diff * lightCol;

    // 镜面反射
    float specInt = 0.5;
    vec3 vDirWS = normalize(camPos - posWS);
    vec3 lrDirWS = reflect(-lightDir, nDirWS);
    float spec = pow(max(dot(lrDirWS, vDirWS), 0.0f), 64.0f);
    vec3 specular = spec * specInt * lightCol;

    // 最终输出结果
    vec3 finalRGB = (ambient + diffuse + specular) * baseCol;
    FragColor = vec4(finalRGB, 1.0f);


}