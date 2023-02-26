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

struct Material
{
	int type;
	vec3 color;
};

struct HitRecord
{
	vec3 point;
	vec3 normal;
	float t;

	Material material;
};

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct AABB
{
    vec3 min;
    vec3 max;
	vec3 normal;
};
uniform AABB aabbs[100];

struct BVH
{
	AABB aabb;
	int offset;
	int count;
};
uniform BVH bvhs[50];

uniform vec3 viewPos;
uniform vec2 canvas;
uniform mat4 inverseCamera;
uniform mat4 modelsMatrix[5];

uniform Material materials[5];

uniform int samples;

layout(location = 0) uniform sampler2D positions;
layout(location = 1) uniform sampler2D normals;
layout(location = 2) uniform sampler2D albedoSpecular;
layout(location = 3) uniform sampler2D accumulateTexture;

layout(location = 4) uniform sampler2D transformsTex;
layout(location = 5) uniform sampler2DArray texturesTex;
layout(location = 6) uniform sampler2D aabbsTex;
layout(location = 7) uniform sampler2D objectsTex;

float nrand(vec2 n)
{
    return fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

float trand(vec2 n, float seed)
{
    return nrand(n * seed * float(samples));
}

vec3 randomPointInUnitSphere(vec2 uv, float seed)
{
    // this is doing rejection sampling, which is simple but not super
    // efficient
    vec3 p;
    do {
        p = vec3(trand(uv, seed),
                 trand(uv, 2.333 * seed),
                 trand(uv, -1.134 * seed));
        seed *= 1.312;
    } while (dot(p, p) >= 1.0);
    return p;
}

vec3 GetRayAt(const Ray ray, float t)
{
	return ray.origin + t * ray.direction;
}

bool Intersection2(Ray ray, inout HitRecord hit, float minT, float maxT)
{
	bool somethingHit = false;
	float closest = maxT;

	for (int i = 0; i < 2; ++i)
	{
		vec4 objectInfo = texelFetch(objectsTex, ivec2(3, 0), 0).xyzw;

		vec4 r1 = texelFetch(transformsTex, ivec2(i * 4, 0), 0).xyzw;
		vec4 r2 = texelFetch(transformsTex, ivec2(i * 4 + 1, 0), 0).xyzw;
		vec4 r3 = texelFetch(transformsTex, ivec2(i * 4 + 2, 0), 0).xyzw;
		vec4 r4 = texelFetch(transformsTex, ivec2(i * 4 + 3, 0), 0).xyzw;

		mat4 modelMatrix = mat4(r1, r2, r3, r4);

		for (int j = int(objectInfo.y); j < int(objectInfo.z); ++j)
		{
			AABB aabb = aabbs[j];

			//aabb.min = texelFetch(aabbsTex, ivec2(j * 4, 0), 0).xyz;
			//aabb.max = texelFetch(aabbsTex, ivec2(j * 4 + 1, 0), 0).xyz;
			//aabb.normal = texelFetch(aabbsTex, ivec2(j * 4 + 3, 0), 0).xyz;

			//aabb.min = 

            aabb.min = vec3(modelMatrix * vec4(aabb.min, 1.0));
			aabb.max = vec3(modelMatrix * vec4(aabb.max, 1.0));

			vec3 invDir = 1.0 / ray.direction;

    		vec3 f = (aabb.max - ray.origin) * invDir;
    		vec3 n = (aabb.min - ray.origin) * invDir;

    		vec3 tMax = max(f, n);
    		vec3 tMin = min(f, n);

    		float t1 = min(tMax.x, min(tMax.y, tMax.z));
    		float t0 = max(tMin.x, max(tMin.y, tMin.z));

			if (t1 < t0)
				continue;

			if (t0 < minT || closest < t0)
			{
				if (t1 < minT || closest < t1)
				{
					continue;
				}
			}

			if (t0 > 0.0) hit.t = t0;
			else hit.t = t1;

			closest = hit.t;
			hit.point = GetRayAt(ray, hit.t);
			hit.normal = dot(ray.direction, aabb.normal) < 0.0 ? aabb.normal : -aabb.normal;
			hit.material = materials[i];
			somethingHit = true;
		}
	}

	return somethingHit;
}

bool Intersection(Ray ray, inout HitRecord hit, float minT, float maxT)
{
	bool somethingHit = false;
	float closest = maxT;

	for (int i = 0; i < 50; ++i)
	{
		BVH bvh = bvhs[i];

		vec4 r1 = texelFetch(transformsTex, ivec2(i * 4, 0), 0).xyzw;
		vec4 r2 = texelFetch(transformsTex, ivec2(i * 4 + 1, 0), 0).xyzw;
		vec4 r3 = texelFetch(transformsTex, ivec2(i * 4 + 2, 0), 0).xyzw;
		vec4 r4 = texelFetch(transformsTex, ivec2(i * 4 + 3, 0), 0).xyzw;

		mat4 modelMatrix = mat4(r1, r2, r3, r4);

        bvh.aabb.min = vec3(modelMatrix * vec4(bvh.aabb.min, 1.0));
		bvh.aabb.max = vec3(modelMatrix * vec4(bvh.aabb.max, 1.0));

		vec3 invDir = 1.0 / ray.direction;

    	vec3 globalF = (bvh.aabb.max - ray.origin) * invDir;
    	vec3 globalN = (bvh.aabb.min - ray.origin) * invDir;

    	vec3 globalTMax = max(globalF, globalN);
    	vec3 globalTMin = min(globalF, globalN);

    	float globalT1 = min(globalTMax.x, min(globalTMax.y, globalTMax.z));
    	float globalT0 = max(globalTMin.x, max(globalTMin.y, globalTMin.z));

		if (globalT1 < globalT0)
			continue;

		if (globalT0 < minT || closest < globalT0)
		{
			if (globalT1 < minT || closest < globalT1)
			{
				continue;
			}
		}

		for (int j = bvh.offset; j < bvh.offset + bvh.count; ++j)
		{
            AABB aabb = aabbs[j];

            aabb.min = vec3(modelMatrix * vec4(aabb.min, 1.0));
			aabb.max = vec3(modelMatrix * vec4(aabb.max, 1.0));

			vec3 invDir = 1.0 / ray.direction;

    		vec3 f = (aabb.max - ray.origin) * invDir;
    		vec3 n = (aabb.min - ray.origin) * invDir;

    		vec3 tMax = max(f, n);
    		vec3 tMin = min(f, n);

    		float t1 = min(tMax.x, min(tMax.y, tMax.z));
    		float t0 = max(tMin.x, max(tMin.y, tMin.z));

			if (t1 < t0)
				continue;

			if (t0 < minT || closest < t0)
			{
				if (t1 < minT || closest < t1)
				{
					continue;
				}
			}

			if (t0 > 0.0) hit.t = t0;
			else hit.t = t1;

			closest = hit.t;
			hit.point = GetRayAt(ray, hit.t);
			hit.normal = dot(ray.direction, aabb.normal) < 0.0 ? aabb.normal : -aabb.normal;
			hit.material = materials[i];
			somethingHit = true;
		}
	}

	return somethingHit;
}

vec3 TracePath(const Ray ray, vec2 uv)
{
	vec3 color = vec3(1.0);
	HitRecord hit;
	Ray hitRay = ray;
	float tMax = 1000000.0;

	float newSeed = 1.0;
	float accum = 1.0;
	
	int i = 0;
	for (; i < 50; ++i)
	{
		if (Intersection2(hitRay, hit, 0.001, tMax))
		{
			hitRay.origin = hit.point;
			hitRay.direction = hit.normal + randomPointInUnitSphere(uv, 1.0);
			tMax = hit.t;
			color *= texture(texturesTex, vec3(0.5, 0.5, 2.0)).rgb;
			newSeed *= 1.456;
		}
		else
		{
			float t = 0.5 * (normalize(hitRay.direction).y + 1.0);
			color *= ((1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0));
			break;
		}
	}

	return color;
}

vec3 GetRayDirection(const vec3 pos, const vec2 fragCoord, float rx, float ry)
{
    float x = ((fragCoord.x + rx) / canvas.x) * 2.0 - 1.0;
    float y = ((fragCoord.y + ry) / canvas.y) * 2.0 - 1.0;

    return vec3(inverseCamera * normalize(vec4(x, y, -1.0, 1.0)));
}

void main()
{
    Ray ray;
    ray.origin = viewPos;

	vec3 color;
	vec3 prev = texture(accumulateTexture, texCoord).rgb;

	vec2 fragCoord = gl_FragCoord.xy;
	vec2 uv = fragCoord / canvas;

	float rx = trand(uv, 0.123);
	float ry = trand(uv, 0.456);

    ray.direction = GetRayDirection(ray.origin, fragCoord, rx, ry);

	color = TracePath(ray, uv);

	color = pow(color, vec3(1.0 / 2.2));
	
	if (prev != vec3(0.0, 0.0, 0.0))
	{
		color = (float(samples - 1) * prev + color) / float(samples);
	}

	//color = texture(albedoSpecular, texCoord).rgb;

    fragColor = vec4(color, 1.0);
}