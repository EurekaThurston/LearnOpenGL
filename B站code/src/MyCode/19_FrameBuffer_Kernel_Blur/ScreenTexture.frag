#version 330 core
out vec4 FragColor;

in vec2 UV;

uniform sampler2D screenTexture;

const float offset = 1.0 / 450.0;

void main()
{ 
    // 每个核值所对应的像素距离中心像素的偏移量
    vec2 offsets[25] = vec2[] (
        // 第一行
        vec2(-2*offset, 2*offset),
        vec2(-offset, 2*offset),
        vec2(0.0, 2*offset),
        vec2(offset, 2*offset),
        vec2(2*offset, 2*offset),
        // 第二行
        vec2(-2*offset, offset),
        vec2(-offset,  offset), 
        vec2(    0.0,  offset), 
        vec2( offset,  offset), 
        vec2(2*offset, offset),
        // 第三行
        vec2(-2*offset, 0.0),
        vec2(-offset,     0.0), 
        vec2(    0.0,     0.0), 
        vec2( offset,     0.0), 
        vec2(2*offset, 0.0),
        // 第四行
        vec2(-2*offset, -offset),
        vec2(-offset, -offset), 
        vec2(    0.0, -offset), 
        vec2( offset, -offset),
        vec2(2* offset, -offset),
        // 第五行
        vec2(-2*offset, -2*offset),
        vec2(-offset, -2*offset),
        vec2(0.0, -2*offset),
        vec2(offset, -2*offset),
        vec2(2*offset, -2*offset)
    );

    // 核矩阵(模糊)
    float kernel[25] = float[](
        1.0/273.0, 4.0/273.0, 7.0/273.0, 4.0/273.0, 1.0/273.0,
        4.0/273.0, 16.0/273.0, 26.0/273.0, 16.0/273.0, 4.0/273.0,
        7.0/273.0, 26.0/273.0, 41.0/273.0, 26.0/273.0, 7.0/273.0,
        4.0/273.0, 16.0/273.0, 26.0/273.0, 16.0/273.0, 4.0/273.0,
        1.0/273.0, 4.0/273.0, 7.0/273.0, 4.0/273.0, 1.0/273.0
    );

    // 通过偏移量将中心像素以及周围的像素采样到sampleTex里
    vec3 sampleTex[25];
    for (int i = 0; i < 25; i++) {
        sampleTex[i] = vec3(texture(screenTexture, UV.st + offsets[i]));
    }

    // 中心像素最终的颜色
    vec3 col = vec3(0.0);
    for (int i = 0; i < 25; i++) {
        col += kernel[i] * sampleTex[i];
    }

    FragColor =vec4(col, 1.0);
}