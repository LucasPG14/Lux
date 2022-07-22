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
	texCoord = aTexCoord;
    normal = aNormals;
	gl_Position = viewProjection * model * vec4(aPosition, 1.0);
	fragPos = aPosition;

    vec3 t = normalize(aTangents);
    vec3 n = normalize(aNormals);

    t = normalize(t - dot(t, n) * n);
    vec3 b = cross(n, t);

    tbn = mat3(t, b, n);
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
}; 
uniform Material material;

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;
in mat3 tbn;

void main()
{
    position.rgb = fragPos;

    normals.rgb = texture(material.normalMap, texCoord).rgb;
    normals.rgb = normals.rgb * 2.0 - 1.0;
    normals.rgb = normalize(normals.rgb);

    albedoSpecular = vec4(texture(material.diffuseMap, texCoord).rgb * material.albedoColor, 1.0);
}