#type vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aTangents;
layout(location = 4) in vec3 aBitangents;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;			

out vec2 texCoord;
			
void main()
{
	texCoord = aTexCoord;
	gl_Position = projection * view * model * vec4(aPosition, 1.0);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 texCoord;

uniform sampler2D ourTexture;

void main()
{
	color = texture(ourTexture, texCoord);
}