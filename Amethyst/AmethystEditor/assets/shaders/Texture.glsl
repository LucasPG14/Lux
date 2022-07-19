#type vertex
#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangents;
layout(location = 4) in vec3 aBitangents;

uniform mat4 viewProjection;
uniform mat4 model;			

out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;
out mat3 tbn;
out mat4 vModel;
			
void main()
{
	texCoord = aTexCoord;
    normal = aNormals;

	gl_Position = viewProjection * model * vec4(aPosition, 1.0);

    vec3 t = normalize(vec3(model * vec4(aTangents,   0.0)));
    vec3 b = normalize(vec3(model * vec4(aBitangents, 0.0)));
    vec3 n = normalize(vec3(model * vec4(aNormals,    0.0)));
    tbn = transpose(mat3(t, b, n));
	
    fragPos = tbn * vec3(model * vec4(aPosition, 1.0));
}


#type fragment
#version 450 core

layout(location = 0) out vec4 fragColor;

uniform vec3 viewPos;

struct Material
{
    vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float shininess;
};
uniform Material material;

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
};
uniform PointLight pointLight; 

layout(location = 0) uniform sampler2D diffuseMap;
layout(location = 1) uniform sampler2D normalMap;

in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;
in mat3 tbn;
in mat4 vModel;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);
    
    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = tbn * normalize(light.position - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    
    float diff = max(dot(normal, lightDir), 0.0);
    
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), material.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (distance * distance);

    vec3 ambient = light.ambient * material.diffuse;
    vec3 diffuse = light.diffuse * diff * material.diffuse;
    vec3 specular = light.specular * spec;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

void main()
{
    vec3 vPos = tbn * viewPos;
    vec3 norm = normalize(texture(normalMap, texCoord).rgb * 2.0 - 1.0);
	
    vec3 viewDir = tbn * normalize(vPos - fragPos);
    
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    result += CalcPointLight(pointLight, norm, fragPos, viewDir);

    fragColor = texture(diffuseMap, texCoord) * vec4(result, 1.0);
}