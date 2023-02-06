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

struct Sphere
{
	vec3 center;
	float radius;
};

struct Scene
{
	Sphere spheres[2];
};

uniform vec3 viewPos;
uniform vec2 canvas;
uniform float verticalFov;
uniform float horizontalFov;
uniform mat4 inverseCamera;
uniform mat4 modelsMatrix[5];

uniform Material materials[5];

uniform float aspectRatio;
uniform float scale;

uniform int samples;
uniform float seed;

layout(location = 0) uniform sampler2D positions;
layout(location = 1) uniform sampler2D normals;
layout(location = 2) uniform sampler2D albedoSpecular;
layout(location = 3) uniform sampler2D accumulateTexture;

vec3 GetRayAt(const Ray ray, float t)
{
	return ray.origin + t * ray.direction;
}

bool Intersection(Ray ray, inout HitRecord hit, float minT, float maxT)
{
	bool somethingHit = false;

	for (int i = 0; i < 50; ++i)
	{
		BVH bvh = bvhs[i];

        bvh.aabb.min = vec3(modelsMatrix[i] * vec4(bvh.aabb.min, 1.0));
		bvh.aabb.max = vec3(modelsMatrix[i] * vec4(bvh.aabb.max, 1.0));

		vec3 invDir = 1.0 / ray.direction;

    	vec3 globalF = (bvh.aabb.max - ray.origin) * invDir;
    	vec3 globalN = (bvh.aabb.min - ray.origin) * invDir;

    	vec3 globalTMax = max(globalF, globalN);
    	vec3 globalTMin = min(globalF, globalN);

    	float globalT1 = min(globalTMax.x, min(globalTMax.y, globalTMax.z));
    	float globalT0 = max(globalTMin.x, max(globalTMin.y, globalTMin.z));

		if (globalT1 < globalT0)
			continue;

		if (globalT0 < minT || maxT < globalT0)
		{
			if (globalT1 < minT || maxT < globalT1)
			{
				continue;
			}
		}

		for (int j = bvh.offset; j < bvh.offset + bvh.count; ++j)
		{
            AABB aabb = aabbs[j];

            aabb.min = vec3(modelsMatrix[i] * vec4(aabb.min, 1.0));
			aabb.max = vec3(modelsMatrix[i] * vec4(aabb.max, 1.0));

			vec3 invDir = 1.0 / ray.direction;

    		vec3 f = (aabb.max - ray.origin) * invDir;
    		vec3 n = (aabb.min - ray.origin) * invDir;

    		vec3 tMax = max(f, n);
    		vec3 tMin = min(f, n);

    		float t1 = min(tMax.x, min(tMax.y, tMax.z));
    		float t0 = max(tMin.x, max(tMin.y, tMin.z));

			if (t1 < t0)
				continue;

			if (t0 < minT || maxT < t0)
			{
				if (t1 < minT || maxT < t1)
				{
					continue;
				}
			}

			if (t0 > 0.0) hit.t = t0;
			else hit.t = t1;

			maxT = hit.t;
			hit.point = GetRayAt(ray, hit.t);
			hit.normal = dot(ray.direction, aabb.normal) < 0 ? aabb.normal : -aabb.normal;
			hit.material = materials[i];
			somethingHit = true;
		}
	}

	return somethingHit;
}

bool Intersection2(Ray ray, inout HitRecord hit, float minT, float maxT, vec3 center, float radius)
{
	vec3 oc = ray.origin - center;
    float a = dot(ray.direction, ray.direction);
    float b = dot(oc, ray.direction);
    float c = dot(oc, oc) - radius * radius;

    float discriminant = b * b - a * c;
    if (discriminant < 0.0) return false;

	float sqrtDiscriminant = sqrt(discriminant);

	float root = (-b - sqrtDiscriminant) / a;
    if (root < minT || maxT < root) 
	{
        root = (-b + sqrtDiscriminant) / a;
        if (root < minT || maxT < root)
            return false;
    }

    hit.t = root;
    hit.point = GetRayAt(ray, hit.t);
    hit.normal = (hit.point - center) / radius;

	bool frontFace = dot(ray.direction, hit.normal) < 0.0;
	hit.normal = frontFace ? hit.normal : -hit.normal;

    return true;
}

float hash1(inout float seed) 
{
    return fract(sin(seed += 0.1)*43758.5453123);
}

vec2 hash2(inout float seed) 
{
    return fract(sin(vec2(seed+=0.1,seed+=0.1))*vec2(43758.5453123,22578.1459123));
}

vec3 hash3(inout float seed) 
{
    return fract(sin(vec3(seed+=0.1,seed+=0.1,seed+=0.1))*vec3(43758.5453123,22578.1459123,19642.3490423));
}

vec3 randomSphereDirection(inout float seed) 
{
    vec2 h = hash2(seed) * vec2(2.0, 6.28318530718) - vec2(1.0, 0.0);
    float phi = h.y;
	return vec3(sqrt(1.0 - h.x * h.x) * vec2(sin(phi), cos(phi)), h.x);
}

vec3 randomHemisphereDirection( const vec3 n, inout float seed ) 
{
	vec3 dr = randomSphereDirection(seed);
	return dot(dr,n) * dr;
}

bool CheckWorld(Ray ray, Scene scene, inout HitRecord hit, float tMin, float tMax)
{
	bool somethingHit = false;
	HitRecord tempHit;
	float closest = tMax;

	for (int i = 0; i < 2; ++i)
	{
		if (Intersection2(ray, tempHit, tMin, closest, scene.spheres[i].center, scene.spheres[i].radius))
		{
			somethingHit = true;
			closest = tempHit.t;
			hit = tempHit;
		}
	}

	return somethingHit;
}

vec3 TracePath(const Ray ray)
{
	vec3 color = vec3(0.0);
	HitRecord hit;
	Ray hitRay = ray;
	float tMax = 1000000.0;

	float newSeed = 0.0;

	float accum = 1.0;

	Scene scene;
	scene.spheres[0].center = vec3(0.0, 0.0, -1.0);
	scene.spheres[0].radius = 0.5;

	scene.spheres[1].center = vec3(0.0, -100.5, -1.0);
	scene.spheres[1].radius = 100.0;

	int i = 0;
	for(; i < 50; ++i)
	{
		if (CheckWorld(hitRay, scene, hit, 0.0, tMax))
		{
			//color = 0.5 * (hit.normal + vec3(1.0, 1.0, 1.0));
			vec3 target = normalize(hit.normal + randomSphereDirection(newSeed));
			newSeed *= 1.456;
			//return 0.5 * ray_color(ray(rec.p, target - rec.p), world);
			hitRay.origin = hit.point;
			hitRay.direction = target;
			accum *= 0.5;
		}
		else
		{
			float t = 0.5 * (normalize(hitRay.direction).y + 1.0);
			color = (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
			break;
		}
	}

	if (i==5)
		return vec3(0.0);

	return color * accum;
//    	if (Intersection(hitRay, hit, 0.0, tMax))
//    	{
//			hitRay.origin = hit.point;
//			hitRay.direction = hit.point + randomHemisphereDirection(hit.normal, newSeed);
//			hitRay.direction -= hit.point;
//			tMax = hit.t;
//    	}
//		else
//		{
//			float t = 0.5 * (normalize(hitRay.direction).y + 1.0);
//			colors[i] = ((1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0));
//			break;
//		}
	//}

	//return color;
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

    //vec3 color = texture(accumulateTexture, texCoord).rgb;
    //vec3 color = vec3(0.0);

	vec3 color = TracePath(ray);
//
//	float scaleFactor = 1.0 / float(50.0);
//	color *= sqrt(scaleFactor);

    fragColor = vec4(color, 1.0);
}