#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth);
void main() {
    float depth = LinearizeDepth(gl_FragCoord.z);
    FragColor = vec4(vec3(depth), 1.0);
};
float LinearizeDepth(float depth) {
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));
}