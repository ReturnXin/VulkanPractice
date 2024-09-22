#version 330 core

layout(location = 0) out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

uniform vec3 viewPos;

uniform sampler2D albedoMap; // 颜色
uniform sampler2D aoMap;
uniform sampler2D metallicMap; // 金属度
uniform sampler2D normalMap;
uniform sampler2D roughnessMap; // 粗糙度

uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

const float PI = 3.1415926;

vec3 fresnelSchlick(float cosThelta, vec3 F0) {
    return F0 + (1 - F0) * pow(clamp((1 - cosThelta), 0.0, 1.0), 5);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometerySchlickGGX(float NdotV, float roughness) {
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);

    float ggx1 = GeometerySchlickGGX(NdotV, roughness);
    float ggx2 = GeometerySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 getNormalFromMap() {
    vec3 tangentNormal = texture(normalMap, fs_in.TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1 = dFdx(fs_in.FragPos);
    vec3 Q2 = dFdy(fs_in.FragPos);
    vec2 st1 = dFdx(fs_in.TexCoords);
    vec2 st2 = dFdy(fs_in.TexCoords);

    vec3 N = normalize(fs_in.Normal);
    vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
    vec3 B = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

void main() {
    vec3 normal = getNormalFromMap();
    vec3 albedo = texture(albedoMap, fs_in.TexCoords).rgb;
    float metallic = texture(metallicMap, fs_in.TexCoords).r;
    float ao = texture(aoMap, fs_in.TexCoords).r;
    float roughness = texture(roughnessMap, fs_in.TexCoords).r;

    vec3 N = normal;
    vec3 V = normalize(viewPos - fs_in.FragPos);
    vec3 R = reflect(-V, N);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);

    // 直接光照
    for(int i = 0; i < 4; i++) {
        vec3 L = normalize(lightPositions[i] - fs_in.FragPos);
        vec3 H = normalize(V + L);

        float distance = length(lightPositions[i] - fs_in.FragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColors[i] * attenuation;

        vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;// 金属不会折射光线，因此不会有漫反射，如果表面为金属，kD为0

        vec3 nominator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = nominator / denominator;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
        // Lo += (kD * albedo / PI ) * NdotL;
    }

    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD += 1.0 - metallic;

    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

    // 预过滤
    const float MAX_RELECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_RELECTION_LOD).rgb;

    // BRDF
    vec2 envBRDF = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);

    vec3 ambient = (kD * diffuse + specular) * ao;

    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));
    // color = pow(color, vec3(1.0 / 2.2));

    FragColor = vec4(color, 1.0);
}