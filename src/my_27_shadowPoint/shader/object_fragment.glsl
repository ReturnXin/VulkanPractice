#version 330 core
out vec4 FragColor;
in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform sampler2D diffuseTetxure;
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;

float ShadowCalculation(vec3 fragPos) {
    vec3 fragToLight = fragPos - lightPos;
    float closestDepth = texture(depthMap, fragToLight).r;
    closestDepth *= far_plane;
    float currentDepth = length(fragToLight);
    float bias = 0.05;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);
    return shadow;
}

void main() {
    vec3 color = texture(diffuseTetxure, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);

    vec3 ambient = 0.3 * color;

    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * color;

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);

    vec3 sepecular = spec * lightColor * color;

    float shadow = ShadowCalculation(fs_in.FragPos);
    
    vec3 result = ambient + (1.0 - shadow) * (diffuse + sepecular);
    FragColor = vec4(result, 1.0);
};