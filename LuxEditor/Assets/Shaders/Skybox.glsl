#type vertex
#version 450 core

layout (location = 0) in vec3 aPosition;

out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    texCoords = aPosition;
    vec4 pos = projection * view * vec4(aPosition, 1.0);
    gl_Position = pos.xyww;
}  

#type fragment
#version 450 core

layout(location = 0) out vec4 positions;
layout(location = 1) out vec4 normals;
layout(location = 2) out vec4 albedoSpecular;

in vec3 texCoords;

uniform samplerCube skybox;

void main()
{    
    positions.rgb = texCoords.rgb;
    albedoSpecular = texture(skybox, texCoords);
}