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

in vec2 texCoord;

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
uniform vec2 canvas;
uniform float verticalFov;
uniform float horizontalFov;
uniform mat4 inverseCamera;
uniform mat4 modelMatrix;

uniform float aspectRatio;
uniform float scale;

layout(location = 0) uniform sampler2D positions;
layout(location = 1) uniform sampler2D normals;
layout(location = 2) uniform sampler2D albedoSpecular;

float Intersection(Ray ray)
{
	for (int i = 0; i < 100; ++i)
	{
		AABB aabb = aabbs[i];

		aabb.min = vec3(modelMatrix * vec4(aabb.min, 1.0));
		aabb.max = vec3(modelMatrix * vec4(aabb.max, 1.0));

		//ray.origin = vec3(inverse(modelMatrix) * vec4(ray.origin, 1.0));
		//ray.direction = vec3(inverse(modelMatrix) * vec4(ray.direction, 1.0));

		vec3 invDir = 1.0 / ray.direction;

    	vec3 f = (aabb.max - ray.origin) * invDir;
    	vec3 n = (aabb.min - ray.origin) * invDir;

    	vec3 tMax = max(f, n);
    	vec3 tMin = min(f, n);

    	float t1 = min(tMax.x, min(tMax.y, tMax.z));
    	float t0 = max(tMin.x, max(tMin.y, tMin.z));

		if (t1 < t0)
			continue;

		if (t0 > 0.0f)
			return t0;

		return t1;
	}

	return -1.0;
}


vec3 TracePath(const Ray ray)
{
    if (Intersection(ray) > 0.0)
    {
        return vec3(1.0, 0.0, 0.0);
    }

    return vec3(0.0);
}

vec3 GetRayDirection(const vec3 pos)
{
    vec2 fragCoord = gl_FragCoord.xy;

    float x = ((fragCoord.x + 0.5) / canvas.x) * 2.0 - 1.0;
    float y = ((fragCoord.y + 0.5) / canvas.y) * 2.0 - 1.0;

    //x = x * sin(horizontalFov / 2.0);
    //y = y * sin(verticalFov / 2.0);

    return vec3(inverseCamera * normalize(vec4(x, y, -1.0, 1.0)));
}

void main()
{
    Ray ray;
    ray.origin = viewPos;
    ray.direction = GetRayDirection(ray.origin);

    vec3 color = texture(albedoSpecular, texCoord).rgb;

    color = TracePath(ray);

    fragColor = vec4(color, 1.0);
}