#version 330 core
out vec4 FragColor;

in vec3 nDir;
in vec3 posVS;

uniform vec3 baseCol;
uniform vec3 lightCol;
uniform vec3 lightPos;
uniform vec3 camPos; 
uniform mat4 M;
uniform mat4 V;

void main() {
    // 观察空间变量
    vec3 lightPosVS = (V * vec4(lightPos, 1.0f)).xyz;
    vec3 nDirVS = normalize(nDir);
    


    // 环境光照
    float ambientInt = 0.1;
    vec3 ambient = ambientInt * lightCol;

    // 漫反射
    vec3 lightDirVS = normalize(lightPosVS - posVS);
    float diff = max(dot(nDirVS, lightDirVS), 0.0f);
    vec3 diffuse = diff * lightCol;

    // 镜面反射
    float specInt = 0.5;
    vec3 vDirVS = normalize(-posVS);
    vec3 lrDirVS = reflect(-lightDirVS, nDirVS);
    float spec = pow(max(dot(lrDirVS, vDirVS), 0.0f), 64.0f);
    vec3 specular = spec * specInt * lightCol;

    // 最终输出结果
    vec3 finalRGB = (ambient + diffuse + specular) * baseCol;
    FragColor = vec4(finalRGB, 1.0f);


}