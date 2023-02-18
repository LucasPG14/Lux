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
out vec3 fragPos;
out vec3 normal;
out mat3 tbn;
out mat4 vModel;
			
void main()
{
    vec4 worldPos = model * vec4(aPosition, 1.0);
	texCoord = aTexCoord;
    fragPos = worldPos.xyz;

    normal = aNormals;
	
    gl_Position = viewProjection * worldPos;
}


#type fragment
#version 450 core

layout(location = 0) out vec4 position;
layout(location = 1) out vec4 normals;
layout(location = 2) out vec4 albedoSpecular;

struct Material
{
    sampler2D diffuseMap;
    vec3 albedoColor;

    sampler2D normalMap;
    sampler2D metallicMap;
    sampler2D roughnessMap;
}; 
uniform Material material;

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;
in mat3 tbn;

void main()
{
    position.rgb = fragPos;
    position.a = texture(material.metallicMap, texCoord).r;

    normals.rgb = texture(material.normalMap, texCoord).rgb;
    normals.rgb = normalize(normals.rgb * 2.0 - 1.0);
    normals.rgb = normals.rgb;
    
    //albedoSpecular.rgb = texture(material.diffuseMap, texCoord).rgb * material.albedoColor;
    albedoSpecular.rgb = material.albedoColor;
    albedoSpecular.a = texture(material.roughnessMap, texCoord).r;
}