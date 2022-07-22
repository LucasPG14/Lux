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

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dirLight;

struct PointLight 
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float radius;
};
uniform PointLight pointLights[32]; 

layout(location = 0) uniform sampler2D positions;
layout(location = 1) uniform sampler2D normals;
layout(location = 2) uniform sampler2D albedoSpecular;

uniform vec3 viewPos;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 albedo, float materialSpecular)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 32.0);
    
    vec3 ambient = light.ambient * albedo;
    vec3 diffuse = light.diffuse * diff * albedo;
    vec3 specular = light.specular * spec * materialSpecular;
    return (ambient + diffuse + specular);
}


vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, float materialSpecular)
{
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), 32.0);

    float distance = length(light.position - fragPos);
    if(distance < light.radius)
    {
        float attenuation = 1.0 / (distance * distance);

        vec3 ambient = light.ambient * albedo;
        vec3 diffuse = light.diffuse * diff * albedo;
        vec3 specular = light.specular * spec * materialSpecular;

        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;

        return (ambient + diffuse + specular);
    }

    return vec3(0.0, 0.0, 0.0);
}


void main()
{
	vec3 fragPos = texture(positions, texCoord).rgb;
	vec3 normal = texture(normals, texCoord).rgb;
	vec3 albedo = texture(albedoSpecular, texCoord).rgb;
	float specular = texture(albedoSpecular, texCoord).a;

	vec3 viewDir = normalize(viewPos - fragPos);

	vec3 light = CalcDirLight(dirLight, normal, viewDir, albedo, specular);

    for (int i = 0; i < 32; ++i)
    {
	    light += CalcPointLight(pointLights[i], normal, fragPos, viewDir, albedo, specular);
    }

    fragColor = vec4(light, 1.0);
}