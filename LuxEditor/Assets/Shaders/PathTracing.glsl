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

	vec2 texCoords;

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

uniform Material materials[5];

uniform int samples;

struct ObjectInfo
{
	vec4 color;
};

layout(std430, binding = 0) buffer verticesSSBO
{
    vec4 vertices[];
};

layout(std430, binding = 1) buffer indicesSSBO
{
    vec4 indices[];
};

layout(std430, binding = 2) buffer objectsSSBO
{
    vec4 objects[];
};

layout(location = 0) uniform sampler2D positions;
layout(location = 1) uniform sampler2D normals;
layout(location = 2) uniform sampler2D albedoSpecular;
layout(location = 3) uniform sampler2D accumulateTexture;

layout(location = 4) uniform sampler2D transformsTex;
layout(location = 5) uniform sampler2DArray texturesTex;
layout(location = 6) uniform sampler2D verticesTex;
layout(location = 7) uniform sampler2D indicesTex;
layout(location = 8) uniform sampler2D normalsTex;
layout(location = 9) uniform sampler2D objectsTex;

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

bool RayTriangleHit(Ray ray, inout HitRecord hit, float minT, float maxT)
{
	bool somethingHit = false;
	float closest = maxT;

	for (int j = 0; j < 7; ++j)
	{
		vec4 object = objects[j];

		vec4 r1 = texelFetch(transformsTex, ivec2(object.w * 4, 0), 0).xyzw;
		vec4 r2 = texelFetch(transformsTex, ivec2(object.w * 4 + 1, 0), 0).xyzw;
		vec4 r3 = texelFetch(transformsTex, ivec2(object.w * 4 + 2, 0), 0).xyzw;
		vec4 r4 = texelFetch(transformsTex, ivec2(object.w * 4 + 3, 0), 0).xyzw;

		mat4 modelMatrix = mat4(r1, r2, r3, r4);

		//vec3 origin = vec3(modelMatrix * vec4(ray.origin, 1.0));
		//vec3 direction = vec3(modelMatrix * vec4(ray.direction, 1.0));
		vec3 origin = ray.origin;
		vec3 direction = ray.direction;

	for (int i = 0; i < 24; ++i)
	{
		//vec3 indices = texelFetch(indicesTex, ivec2(i, 0), 0).xyz;

		//vec3 v1 = texelFetch(verticesTex, ivec2(indices.x, 0), 0).xyz;
		//vec3 v2 = texelFetch(verticesTex, ivec2(indices.y, 0), 0).xyz;
		//vec3 v3 = texelFetch(verticesTex, ivec2(indices.z, 0), 0).xyz;

		//vec4 indicesTriangle = indices[i];
		vec3 v1 = vertices[int(indices[i].x)].xyz;
		vec3 v2 = vertices[int(indices[i].y)].xyz;
		vec3 v3 = vertices[int(indices[i].z)].xyz;

		v1 = vec3(modelMatrix * vec4(v1, 1.0));
		v2 = vec3(modelMatrix * vec4(v2, 1.0));
		v3 = vec3(modelMatrix * vec4(v3, 1.0));

		vec3 v1v2 = v2 - v1;
    	vec3 v1v3 = v3 - v1;

		// OPTIMIZED VERSION
		vec3 point = cross(direction, v1v3);
		float det = dot(v1v2, point);

		if (det < 0.0003)
			continue;

		float invDet = 1.0 / det;

		vec3 tVector = origin - v1;
		float u = dot(tVector, point) * invDet;
		if (u < 0.0 || u > 1.0) 
			continue;

		vec3 qVector = cross(tVector, v1v2);
		float v = dot(direction, qVector) * invDet;
		if (v < 0.0 || u + v > 1.0) 
			continue;

		float t = dot(v1v3, qVector) * invDet;

//		vec3 n = cross(v1v2, v1v3);
//		float area2 = length(n);
//
//		float NdotRayDirection = dot(n, direction);
//		if (abs(NdotRayDirection) < 0.00003) // almost 0
//			continue;
//
//		float d = -dot(n, v1);
//		float t = -(dot(n, origin) + d) / NdotRayDirection;
//
//		if (t < 0.0 || t < minT || closest < t) 
//			continue;
//
//		vec3 point = GetRayAt(ray, t);
//
//		// Edge 0
//		vec3 edge0 = v2 - v1; 
//		vec3 vp0 = point - v1;
//		vec3 c = cross(edge0, vp0);
//		if (dot(n, c) < 0.0) 
//			continue;
//
//		// Edge 1
//		vec3 edge1 = v3 - v2; 
//		vec3 vp1 = point - v2;
//		c = cross(edge1, vp1);
//		float u = length(c) / area2;
//		if (dot(n, c) < 0.0)  
//			continue;
//
//		// Edge 2
//		vec3 edge2 = v1 - v3; 
//		vec3 vp2 = point - v3;
//		c = cross(edge2, vp2);
//		float v = length(c) / area2;
//		if (dot(n, c) < 0.0)  
//			continue;

		if (t > 0.0 && minT < t && t < closest)
		{
			closest = t;
			somethingHit = true;
			hit.t = t;

			hit.point = GetRayAt(ray, t);

			vec2 tC1 = vec2(texelFetch(verticesTex, ivec2(int(indices[i].x), 0), 0).w, texelFetch(normalsTex, ivec2(int(indices[i].x), 0), 0).w);
			vec2 tC2 = vec2(texelFetch(verticesTex, ivec2(int(indices[i].y), 0), 0).w, texelFetch(normalsTex, ivec2(int(indices[i].y), 0), 0).w);
			vec2 tC3 = vec2(texelFetch(verticesTex, ivec2(int(indices[i].z), 0), 0).w, texelFetch(normalsTex, ivec2(int(indices[i].z), 0), 0).w);

			vec2 texCoords = tC1 * (1.0 - u - v) + tC2 * u + tC3 * v;
			hit.texCoords = texCoords;

			hit.material.color = object.xyz;

			vec3 normal1 = texelFetch(normalsTex, ivec2(int(indices[i].x), 0), 0).xyz;
			vec3 normal2 = texelFetch(normalsTex, ivec2(int(indices[i].y), 0), 0).xyz;
			vec3 normal3 = texelFetch(normalsTex, ivec2(int(indices[i].z), 0), 0).xyz;

			hit.normal = normalize(normal1 * (1.0 - u -v) + normal2 * u + normal3 * v);
			hit.normal = dot(ray.direction, hit.normal) < 0.0 ? hit.normal : -hit.normal;
		}
	}
	}

	return somethingHit;
}

bool Intersection2(Ray ray, inout HitRecord hit, float minT, float maxT)
{
	bool somethingHit = false;
	float closest = maxT;

	for (int i = 0; i < 1; ++i)
	{
		vec4 objectInfo = texelFetch(objectsTex, ivec2(i, 0), 0).xyzw;

		vec4 r1 = texelFetch(transformsTex, ivec2(objectInfo.x * 4, 0), 0).xyzw;
		vec4 r2 = texelFetch(transformsTex, ivec2(objectInfo.x * 4 + 1, 0), 0).xyzw;
		vec4 r3 = texelFetch(transformsTex, ivec2(objectInfo.x * 4 + 2, 0), 0).xyzw;
		vec4 r4 = texelFetch(transformsTex, ivec2(objectInfo.x * 4 + 3, 0), 0).xyzw;

		mat4 modelMatrix = mat4(r1, r2, r3, r4);

//		vec3 origin = vec3(inverse(modelMatrix) * vec4(ray.origin, 1.0));
//		vec3 direction = vec3(inverse(modelMatrix) * vec4(ray.direction, 1.0));

		for (int j = int(objectInfo.y); j < int(objectInfo.z); ++j)
		{
			//AABB aabb = aabbs[j];

			AABB aabb;
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

		vec4 objectInfo = texelFetch(objectsTex, ivec2(i, 0), 0).xyzw;

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
		if (RayTriangleHit(hitRay, hit, 0.001, tMax))
		{
			hitRay.origin = hit.point;
			hitRay.direction = hit.normal + randomPointInUnitSphere(uv, 1.0);
			tMax = hit.t;
			color *= hit.material.color;
			//color = vec3(1.0, 0.0, 0.0) ;
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