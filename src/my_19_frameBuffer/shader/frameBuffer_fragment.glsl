#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;
const float offset = 1.0 / 300.0;

void main() {
    // FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)),1.0);
    vec2 offsets[9] = vec2[](vec2(-offset, offset), // 左上
    vec2(0.0, offset), // 正上
    vec2(offset, offset), // 右上
    vec2(-offset, 0.0),   // 左
    vec2(0.0, 0.0),   // 中
    vec2(offset, 0.0),   // 右
    vec2(-offset, -offset), // 左下
    vec2(0.0, -offset), // 正下
    vec2(offset, -offset)  // 右下
    );
    float kernel[9] = float[](1.0 / 16, 2.0 / 16, 1.0 / 16, 2.0 / 16, 4.0 / 16, 2.0 / 16, 1.0 / 16, 2.0 / 16, 1.0 / 16);
    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++) sampleTex[i] = vec3(texture(screenTexture, TexCoords + offsets[i]));
    vec3 col;
    for(int i = 0; i < 9; i++) col += sampleTex[i] * kernel[i];
    FragColor = vec4(col, 1.0);
};