#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
uniform vec3 viewPos;
uniform sampler2D normalMap;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} fs_in;

void main() {
    float gamma = 2.2;
    vec3 ambient = light.ambient * pow(texture(material.diffuse, fs_in.TexCoords).rgb, vec3(gamma));

    vec3 normal = texture(normalMap, fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(fs_in.TBN * normal);

    vec3 norm = normalize(normal);
    // vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - fs_in.FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * pow(texture(material.diffuse, fs_in.TexCoords).rgb, vec3(gamma));

    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 sepecular = spec * light.specular * pow(texture(material.specular, fs_in.TexCoords).rgb, vec3(gamma));

    vec3 result = ambient + diffuse + sepecular;
    FragColor = vec4(result, 1.0);

    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma));
};