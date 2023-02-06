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

layout(location = 0) out vec4 outColor;

layout(location = 0) uniform sampler2D pathColor;

in vec2 texCoord;

uniform int samples;

void main()
{
    vec4 color = texture(pathColor, texCoord);

    float scale = 1.0 / float(samples);
    color *= samples;

    outColor = color;
}