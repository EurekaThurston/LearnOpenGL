#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT {
    vec3 posWS;
    vec3 nDirWS;
    vec2 UV;
} FragmentInput;

struct Light {
    vec3 Position;
    vec3 Color; 
};

uniform Light lights[4];
uniform sampler2D Diffuse;
uniform sampler2D Specular;
uniform vec3 camPos;

void main() {
    vec3 T_diffuse = texture(Diffuse, FragmentInput.UV).rgb;
    vec3 T_specular = texture(Specular, FragmentInput.UV).rgb;
    vec3 nDirWS = normalize(FragmentInput.nDirWS);

    // lighting
    vec3 lighting = vec3(0.0);

    for(int i = 0; i < 4; i++) {
        // 向量准备
        vec3 lDirWS = normalize(lights[i].Position - FragmentInput.posWS);
        vec3 vDirWS = normalize(camPos - FragmentInput.posWS);
        vec3 hDirWS = normalize(lDirWS + vDirWS);

        // 漫反射
        float nDotL = max(0.0, dot(nDirWS, lDirWS));
        vec3 diffuse = nDotL * T_diffuse * lights[i].Color;

        // 镜面反射
        float nDotH = max(0.0, dot(nDirWS, hDirWS));
        vec3 specular = pow(nDotH, 128) * T_specular * lights[i].Color;

        // 环境光
        vec3 ambient = 0.1 * T_diffuse;  

        // 光照衰减
        float dist = length(FragmentInput.posWS - lights[i].Position);

        vec3 finalRGB = diffuse + specular + ambient;
        finalRGB *= 1.0 / (dist * dist);

        lighting += finalRGB;
    }

    // 输出1
    FragColor = vec4(lighting, 1.0);

    // 输出2
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0) {
        BrightColor = vec4(FragColor.rgb, 1.0);
    } else {
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}