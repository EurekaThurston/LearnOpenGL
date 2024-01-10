#version 330 core
out vec4 FragColor;

in vec3 nDir;
in vec3 posWS;
in vec2 UV;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 lightPos;
    vec3 ambientInt;
    vec3 diffuseInt;
    vec3 specularInt;
};

uniform Material material;
uniform Light light;
uniform vec3 baseCol;
uniform vec3 lightCol;
uniform vec3 camPos; 

void main() {
    // 贴图载入
    vec3 var_diff = texture(material.diffuse, UV).rgb;
    vec3 var_spec = 1-texture(material.specular, UV).rgb;
    // 环境光
    vec3 ambient = var_diff;

    // 漫反射
    vec3 nDirWS = normalize(nDir);
    vec3 lDirWS = normalize(light.lightPos - posWS);
    float diff = max(dot(nDirWS, lDirWS), 0.0);
    vec3 diffuse =  diff * var_diff;

    // 镜面反射
    vec3 vDirWS = normalize(camPos - posWS);
    vec3 lrDirWS = reflect(-lDirWS, nDirWS);
    float spec = pow(max(dot(lrDirWS, vDirWS), 0.0), material.shininess);
    vec3 specular =  (spec * var_spec);

    // 最终输出
    ambient *= light.ambientInt;
    diffuse *= light.diffuseInt;
    specular *= light.specularInt;
    vec3 finalRGB = ambient + diffuse + specular;
    FragColor = vec4(finalRGB, 1.0);
}