#version 330 core
layout(location = 0) in vec3 Position;

out VS_OUT {
  out vec3 outFragPos;
}vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
  gl_Position = projection * view * model * vec4(Position, 1.0);
  vs_out.outFragPos = vec3(model * vec4(Position, 1.0));
}