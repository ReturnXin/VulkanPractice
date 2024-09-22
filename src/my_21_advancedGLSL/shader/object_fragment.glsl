#version 330 core
out vec4 FragColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
Material material;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;
uniform vec3 viewPos;
uniform sampler2D frontTexture;
uniform sampler2D backTetxure;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

void main() {
    material.ambient = vec3(gl_FrontFacing? texture(frontTexture,TexCoords):texture(backTetxure,TexCoords));
    material.diffuse = vec3(gl_FrontFacing? texture(frontTexture,TexCoords):texture(backTetxure,TexCoords));
    material.specular = vec3(0.5, 0.5, 0.5);
    material.shininess = 32.0;
    vec3 ambient = light.ambient * material.ambient;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * material.diffuse;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflecDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflecDir), 0.0), material.shininess);
    vec3 sepecular = spec * light.specular * material.specular;

    vec3 result = ambient + diffuse + sepecular;
    FragColor = vec4(result, 1.0);
};