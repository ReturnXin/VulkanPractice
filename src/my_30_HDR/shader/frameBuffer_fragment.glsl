#version 330 core
out vec4 FragColor;
in vec2 TexCoords;
uniform sampler2D screenTexture;
float exposure = 0.01;

void main() {
    const float gamma = 2.2;
    vec3 hdrColor = vec3(texture(screenTexture, TexCoords));
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(mapped, 1.0);
};