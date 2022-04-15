#type vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in float aMatIndex;

uniform mat4 viewProjection;		

out vec2 texCoord;
out float index;
			
void main()
{
	texCoord = aTexCoord;
	index = aMatIndex;
	gl_Position = viewProjection * vec4(aPosition, 1.0);
}


#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 texCoord;
in flat float index

layout (binding = 0) uniform sampler2D texturesArray[32];

void main()
{
	vec4 texColor = vec4(1.0, 1.0, 1.0, 1.0);
	switch((int)index)
	{
		case 0: texColor = texture(texturesArray[0], texCoord); break;
		case 1: texColor = texture(texturesArray[1], texCoord); break;
	}
}