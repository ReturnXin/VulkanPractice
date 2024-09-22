#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

layout(std140) uniform Mtrices {
    mat4 projection;
    mat4 view;
};
uniform mat4 model;

out vec2 TexCoord;

void main() {
    gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    TexCoord = aTexCoord;
};