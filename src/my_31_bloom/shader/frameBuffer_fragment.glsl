#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
float exposure = 0.1;

void main() {
    const float gamma = 2.2;
    vec3 hdrColor = vec3(texture(scene, TexCoords).rgb);
    vec3 bloomColor = vec3(texture(bloomBlur, TexCoords).rgb);
    hdrColor += bloomColor;
    
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    mapped = pow(mapped, vec3(1.0 / gamma));
    FragColor = vec4(mapped, 1.0);
};