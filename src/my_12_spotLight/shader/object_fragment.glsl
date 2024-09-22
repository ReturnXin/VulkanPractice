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
    vec3 direction;
    float cutOff;
    float otherCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
uniform vec3 viewPos;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

void main() {
    vec3 result;
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * vec3(texture(material.diffuse, TexCoords));

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflecDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflecDir), 0.0), material.shininess);
    vec3 sepecular = spec * light.specular * vec3(texture(material.specular, TexCoords));
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.otherCutOff;
    float intensity = clamp((theta - light.otherCutOff) / epsilon, 0.0, 1.0);

    diffuse *= intensity;
    sepecular *= intensity;
    result = (diffuse + sepecular + ambient);

    FragColor = vec4(result, 1.0);
    // FragColor = vec4(0.0, theta, 0.0, 1.0);
};