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

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct AABB
{
    vec3 min;
    vec3 max;
};
uniform AABB aabbs[100];

uniform vec3 viewPos;

void main()
{
    Ray ray;
    ray.origin = viewPos;
    ray.direction;

    fragColor = vec4(ray.origin, 1.0);
}