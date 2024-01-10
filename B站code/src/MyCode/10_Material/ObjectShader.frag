#version 330 core
out vec4 FragColor;

in vec3 nDir;
in vec3 posWS;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 lightPos;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 baseCol;
uniform vec3 lightCol;
uniform vec3 camPos; 

void main() {
    // 环境光
    vec3 ambient = material.ambient;

    // 漫反射
    vec3 nDirWS = normalize(nDir);
    vec3 lDirWS = normalize(light.lightPos - posWS);
    float diff = max(dot(nDirWS, lDirWS), 0.0);
    vec3 diffuse = diff * material.diffuse;

    // 镜面反射
    vec3 vDirWS = normalize(camPos - posWS);
    vec3 lrDirWS = reflect(-lDirWS, nDirWS);
    float spec = pow(max(dot(lrDirWS, vDirWS), 0.0), material.shininess);
    vec3 specular = spec * material.specular;

    // 最终输出
    ambient *= light.ambient;
    diffuse *= light.diffuse;
    specular *= light.specular;
    vec3 finalRGB = ambient + diffuse + specular;
    FragColor = vec4(finalRGB, 1.0);

}