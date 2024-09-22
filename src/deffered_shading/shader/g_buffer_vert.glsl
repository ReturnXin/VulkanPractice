#version 330 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in float Intensity;
// layout(location = 2) in vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    float Intensity;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(Position,1.0f);

    vs_out.FragPos = vec3(model * vec4(Position, 1.0));
    // vs_out.TexCoords = TexCoords;
    vs_out.Normal = mat3(transpose(inverse(model))) * Position;
    vs_out.Intensity = Intensity;
}