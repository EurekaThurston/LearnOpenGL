#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 posWS;
    vec3 nDirWS;
    vec2 UV;
} FragmentInput;

struct Light {
    vec3 Position;
    vec3 Color; 
};

uniform Light lights[16];
uniform sampler2D diffuse;
uniform vec3 vDirWS;

void main() {
    vec3 color = texture(diffuse, FragmentInput.UV).rgb;
    vec3 nDirWS = normalize(FragmentInput.nDirWS);
    // 环境光
    vec3 ambient = 0.0 * color;
    // lighting
    vec3 lighting = vec3(0.0);
    for(int i = 0; i < 16; i++) {
        // diffuse
        vec3 lDirWS = normalize(lights[i].Position - FragmentInput.posWS);
        float nDotL = max(0.0, dot(lDirWS, nDirWS));
        vec3 diffuse = lights[i].Color * nDotL * color;
        vec3 result = diffuse;
        float dist = length(FragmentInput.posWS - lights[i].Position);
        result *= 1.0 / (dist * dist);
        lighting += result;
    }

    // FragColor = vec4(ambient + lighting, 1.0);
    FragColor = vec4(lighting + ambient, 1.0);
    
}