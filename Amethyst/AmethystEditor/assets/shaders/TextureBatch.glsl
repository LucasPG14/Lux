#type vertex
#version 450 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in vec3 aTangents;
layout(location = 4) in vec3 aBitangents;
layout(location = 5) in float aMatIndex;

uniform mat4 viewProjection;		

struct VertexOutput
{
	vec3 normals;
	vec2 texCoords;
	vec3 tangents;
};

layout (location = 0) out VertexOutput Output;
layout (location = 3) out flat float index;
			
void main()
{
	Output.normals = aNormals;
	Output.texCoords = aTexCoords;
	Output.tangents = aTangents;
	index = aMatIndex;
	
	gl_Position = viewProjection * vec4(aPosition, 1.0);
}


#type fragment
#version 450 core

layout(location = 0) out vec4 color;

struct VertexOutput
{
	vec3 normals;
	vec2 texCoords;
	vec3 tangents;
};

layout (location = 0) in VertexOutput Output;
layout (location = 3) in flat float index;

layout (binding = 0) uniform sampler2D texturesArray[32];

void main()
{
	switch(int(index))
	{
		case 0: color = texture(texturesArray[0], Output.texCoords); break;
		case 1: color = texture(texturesArray[1], Output.texCoords); break;
		case 2: color = texture(texturesArray[2], Output.texCoords); break;
	}
}