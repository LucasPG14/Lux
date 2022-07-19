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

    tbn = transpose(mat3(t, b, n));
}


#type fragment
#version 450 core

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normals;
layout(location = 2) out vec4 albedoSpecular;

struct Material
{
    vec3 albedoColor;
}; 
uniform Material material;

in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;
in mat3 tbn;

layout(location = 0) uniform sampler2D diffuseMap;
layout(location = 1) uniform sampler2D normalMap;

void main()
{
    position = tbn * fragPos;
    normals = texture(normalMap, texCoord).rgb;
    normals = normals * 2.0 - 1.0;
    normals = normalize(tbn * normals);

    albedoSpecular = vec4(texture(diffuseMap, texCoord).rgb * material.albedoColor, 1.0);
}