#type vertex
#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;	

out vec2 texCoord;
			
void main()
{
	texCoord = aTexCoord;
	gl_Position = vec4(aPosition, 1.0);
}

#type fragment
#version 450 core

out vec4 fragColor;

in vec2 texCoord;

struct DirLight 
{
    vec3 direction;

    vec3 radiance;
};
uniform DirLight dirLight;

struct PointLight 
{
    vec3 position;

    vec3 radiance;

    float radius;
};
uniform PointLight pointLights[32]; 

struct SpotLight 
{
    vec3 position;
    vec3 direction;

    vec3 radiance;

    float radius;
    float cutOff;
    float outerCutOff;
};
uniform SpotLight spotLights[32];

layout(location = 0) uniform sampler2D positions;
layout(location = 1) uniform sampler2D normals;
layout(location = 2) uniform sampler2D albedoSpecular;

uniform vec3 viewPos;

const float PI = 3.14159265359;


vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - min(cosTheta, 1.0), 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
	
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float denom = NdotV * (1.0 - k) + k;
	
    return NdotV / denom;
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo, float metallic, float roughness, vec3 F0)
{
    vec3 L = normalize(-light.direction);
    vec3 H = normalize(viewDir + L);

    float NdotL = max(dot(normal, L), 0.0);
    float NdotV = max(dot(normal, viewDir), 0.0);

    vec3 radiance = light.radiance;

    float NDF = DistributionGGX(normal, H, roughness);        
    float G = GeometrySmith(NdotV, NdotL, roughness);      
    vec3 F = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * NdotV * NdotL + 0.0001;
    vec3 specular = numerator / denominator;           
    
    return ((kD * albedo / PI + specular) * radiance * NdotL);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float metallic, float roughness, vec3 F0)
{
    vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(viewDir + L);

    float NdotL = max(dot(normal, L), 0.0);
    float NdotV = max(dot(normal, viewDir), 0.0); 

    float distance = length(light.position - fragPos);
    float attenuation = pow(clamp(1.0 - pow(distance / light.radius, 4.0), 0.0, 1.0), 2.0) / ((distance * distance) + 1);
    vec3 radiance = light.radiance * attenuation;

    float NDF = DistributionGGX(normal, H, roughness);        
    float G = GeometrySmith(NdotV, NdotL, roughness);      
    vec3 F = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * NdotV * NdotL + 0.0001;
    vec3 specular = numerator / denominator;
              
    return ((kD * albedo / PI + specular) * radiance * NdotL); 
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float metallic, float roughness, vec3 F0)
{
    vec3 L = normalize(light.position - fragPos);
    vec3 H = normalize(viewDir + L);

    float NdotL = max(dot(normal, L), 0.0);
    float NdotV = max(dot(normal, viewDir), 0.0); 

    // Calculating attenuation
    float distance = length(light.position - fragPos);
    float attenuation = pow(clamp(1.0 - pow(distance / light.radius, 4.0), 0.0, 1.0), 2.0) / ((distance * distance) + 1);

    // Calculating spot light
    float theta = dot(L, normalize(-light.direction)); 
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    vec3 radiance = light.radiance * attenuation * intensity;

    float NDF = DistributionGGX(normal, H, roughness);        
    float G = GeometrySmith(NdotV, NdotL, roughness);      
    vec3 F = FresnelSchlick(max(dot(H, viewDir), 0.0), F0);

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * NdotV * NdotL + 0.0001;
    vec3 specular = numerator / denominator;
              
    return ((kD * albedo / PI + specular) * radiance * NdotL); 
}


void main()
{
	vec3 fragPos = texture(positions, texCoord).rgb;
	vec3 normal = texture(normals, texCoord).rgb;
	vec3 albedo = texture(albedoSpecular, texCoord).rgb;
    float metallic = texture(positions, texCoord).a;
    float roughness = texture(albedoSpecular, texCoord).a;
	
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

	vec3 viewDir = normalize(viewPos - fragPos);

    vec3 Lo = vec3(0.0);
    Lo += CalcDirLight(dirLight, normal, viewDir, albedo, metallic, roughness, F0);
    
    for (int i = 0; i < 32; ++i)
    {
        Lo += CalcPointLight(pointLights[i], normal, fragPos, viewDir, albedo, metallic, roughness, F0);
    }

    for (int i = 0; i < 32; ++i)
    {
        Lo += CalcSpotLight(spotLights[i], normal, fragPos, viewDir, albedo, metallic, roughness, F0);
    }

    vec3 ambient = vec3(0.03) * albedo * 1.0f;
    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0));

    fragColor = vec4(fragPos, 1.0);
}