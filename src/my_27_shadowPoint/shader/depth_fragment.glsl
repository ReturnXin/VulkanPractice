#version 330 core
in vec4 FragPos;
out vec4 FragColor;

uniform vec3 lightPos;
uniform float far_plane;

void main() {
    // 获取片段和光源之间的距离
    float lightDistance = length(FragPos.xyz - lightPos);

    // 通过除以far_plane映射到[0 - 1]范围
    lightDistance = lightDistance / far_plane;

    // 修改深度值
    gl_FragDepth = lightDistance;
    // FragColor = vec4(vec3(lightDistance), 1.0);
    // FragColor = vec4(1.0,1.0,1.0,1.0);
}