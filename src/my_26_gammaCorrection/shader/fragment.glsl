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
uniform bool blinn;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

void main() {
    float gamma = 2.2;
    vec3 ambient = light.ambient * pow(texture(material.diffuse, TexCoords).rgb, vec3(gamma));

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * pow(texture(material.diffuse, TexCoords).rgb, vec3(gamma));

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 sepecular = spec * light.specular * pow(texture(material.specular, TexCoords).rgb, vec3(gamma));

    vec3 result = ambient + diffuse + sepecular;
    FragColor = vec4(result, 1.0);

    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma));
};