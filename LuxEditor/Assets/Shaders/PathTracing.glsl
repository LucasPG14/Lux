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

struct MaterialInfo
{
	vec4 textureIDs;
	vec4 color;
	vec4 properties;
};

struct HitRecord
{
	vec3 point;
	vec3 normal;
	float t;
	bool frontFace;

	vec2 texCoords;

	MaterialInfo material;
};

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct AABB
{
    vec4 min;
    vec4 max;
};
//uniform AABB aabbs[100];

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

uniform int samples;

uniform int numObjects;

layout(std430, binding = 0) buffer verticesSSBO
{
    vec4 vertices[];
};

layout(std430, binding = 1) buffer indicesSSBO
{
    vec4 indices[];
};

layout(std430, binding = 2) buffer normalsSSBO
{
    vec4 normalsBO[];
};

layout(std430, binding = 3) buffer objectsSSBO
{
    vec4 objects[];
};

layout(std430, binding = 4) buffer meshesSSBO
{
    vec4 meshes[];
};

layout(std430, binding = 5) buffer materialsSSBO
{
    MaterialInfo materials[];
};

layout(std430, binding = 6) buffer transformsSSBO
{
    mat4 transforms[];
};

layout(std430, binding = 7) buffer aabbsSSBO
{
    AABB aabbs[];
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

const float PI = 3.14159265;

float nrand(vec2 n)
{
    return fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

float hash(float n) { return fract(sin(n) * 1e4); }

float trand(vec2 n, float seed)
{
    return nrand(n * seed * float(samples));
}

float schlick(float cosTheta, float refractionIdx)
{
	float r0 = (1.0 - refractionIdx) / (1.0 + refractionIdx);
    r0 = r0 * r0;
    return r0 + (1.0 - r0) * pow((1.0 - cosTheta), 5.0);
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

vec3 randomPointInUnitSphere2(vec2 uv, float seed, float minV, float maxV)
{
    vec3 v;
    do {
        v.x = minV + (maxV - minV) * hash(seed);
        v.y = minV + (maxV - minV) * hash(2.333 * seed);
        v.z = minV + (maxV - minV) * hash(-1.134 * seed);
		seed *= 1.312;
    } while(length(v) >= 1.0);

	return v;
}

vec3 GetRayAt(const Ray ray, float t)
{
	return ray.origin + t * ray.direction;
}

bool HitAABB(vec3 origin, vec3 direction, int aabbID, mat4 modelMatrix)
{
	AABB aabb = aabbs[aabbID];

	//vec3 minAABB = vec3(modelMatrix * vec4(aabb.min.xyz, 1.0));
	//vec3 maxAABB = vec3(modelMatrix * vec4(aabb.max.xyz, 1.0));

	vec3 minAABB = aabb.min.xyz;
	vec3 maxAABB = aabb.max.xyz;
	
	vec3 invDir = 1.0 / direction;
    vec3 f = (maxAABB - origin) * invDir;
    vec3 n = (minAABB - origin) * invDir;
    
	vec3 tMax = max(f, n);
    vec3 tMin = min(f, n);
    
	float t1 = min(tMax.x, min(tMax.y, tMax.z));
    float t0 = max(tMin.x, max(tMin.y, tMin.z));
	
	if (t1 < t0)
		return false;
	
	if (t0 > 0.0f)
		return true;
	
	return true;
}

bool RayTriangleHit(Ray ray, inout HitRecord hit, float minT, float maxT)
{
	bool somethingHit = false;
	float closest = maxT;

	for (int j = 0; j < numObjects; ++j)
	{
		vec4 object = objects[j];

		//vec4 r1 = texelFetch(transformsTex, ivec2(object.x * 4, 0), 0).xyzw;
		//vec4 r2 = texelFetch(transformsTex, ivec2(object.x * 4 + 1, 0), 0).xyzw;
		//vec4 r3 = texelFetch(transformsTex, ivec2(object.x * 4 + 2, 0), 0).xyzw;
		//vec4 r4 = texelFetch(transformsTex, ivec2(object.x * 4 + 3, 0), 0).xyzw;

		mat4 modelMatrix = transforms[int(object.x)];
		//mat4 modelMatrix = mat4(r1, r2, r3, r4);

		vec3 origin = vec3(inverse(modelMatrix) * vec4(ray.origin, 1.0));
		vec3 direction = vec3(inverse(modelMatrix) * vec4(ray.direction, 0.0));

		if (!HitAABB(origin, direction, int(object.y), modelMatrix))
			continue;

		vec4 meshInfo = meshes[int(object.y)];
		int meshIndices = int(meshInfo.z + meshInfo.w);

		for (int i = int(meshInfo.z); i < meshIndices; ++i)
		{
			vec4 v1 = vertices[int(indices[i].x + meshInfo.x)];
			vec4 v2 = vertices[int(indices[i].y + meshInfo.x)];
			vec4 v3 = vertices[int(indices[i].z + meshInfo.x)];

			vec3 v1v2 = v2.xyz - v1.xyz;
    		vec3 v1v3 = v3.xyz - v1.xyz;

			// OPTIMIZED VERSION
			vec3 point = cross(direction, v1v3);
			float det = dot(v1v2, point);

			if (det < 0.0003)
				continue;

			float invDet = 1.0 / det;

			vec3 tVector = origin - v1.xyz;
			float u = dot(tVector, point) * invDet;
			if (u < 0.0 || u > 1.0) 
				continue;

			vec3 qVector = cross(tVector, v1v2);
			float v = dot(direction, qVector) * invDet;
			if (v < 0.0 || u + v > 1.0) 
				continue;

			float t = dot(v1v3, qVector) * invDet;

			if (t > 0.0 && minT < t && t < closest)
			{
				closest = t;
				somethingHit = true;
				hit.t = t;

				hit.point = GetRayAt(ray, t);

				// Extracting normals from the SSBO
				vec4 normal1 = normalsBO[int(indices[i].x + meshInfo.x)];
				vec4 normal2 = normalsBO[int(indices[i].y + meshInfo.x)];
				vec4 normal3 = normalsBO[int(indices[i].z + meshInfo.x)];

				vec2 tC1 = vec2(v1.w, normal1.w);
				vec2 tC2 = vec2(v2.w, normal2.w);
				vec2 tC3 = vec2(v3.w, normal3.w);

				hit.texCoords = tC1 * (1.0 - u - v) + tC2 * u + tC3 * v;

				hit.material = materials[int(object.z)];

				hit.normal = normalize(normal1.xyz * (1.0 - u -v) + normal2.xyz * u + normal3.xyz * v);
				hit.normal = normalize(transpose(inverse(mat3(modelMatrix))) * hit.normal);

				hit.frontFace = dot(hit.normal, ray.direction) <= 0.0;
				hit.normal = hit.frontFace ? hit.normal : -hit.normal;
			}
		}
	}

	return somethingHit;
}

vec3 TracePath(const Ray ray, vec2 uv, inout float seed, inout int n)
{
	vec3 color = vec3(0.0);
	Ray hitRay = ray;
	float tMax = 1000000.0;

	float accum = 1.0;

	vec3 throughput = vec3(1.0);
	
	int i = 0;
	for (; i < 8; ++i)
	{
		HitRecord hit;
		hitRay.direction = normalize(hitRay.direction);
		if (RayTriangleHit(hitRay, hit, 0.001, tMax))
		{
			n++;
			if (i == 7)
			{
				color = vec3(0.0);
				return color;
			}
			hitRay.origin = hit.point;

			vec3 randomDir = randomPointInUnitSphere(uv, seed);

			float met = hit.material.properties.x;

			vec3 reflectedDir = reflect(hitRay.direction, hit.normal);
			if (met == 0.5)
			{
				float refractionIdx = hit.material.properties.z;

				float indexRef = hit.frontFace ? refractionIdx : 1.0 / refractionIdx;

            	float cosTheta = min(dot(-hitRay.direction, hit.normal), 1.0);
				float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
           		if (indexRef * sinTheta > 1.0 || nrand(uv * seed) < schlick(cosTheta, indexRef)) 
				{
                	hitRay.direction = reflectedDir;
            	}
				else 
				{
                	hitRay.direction = refract(hitRay.direction, hit.normal, indexRef);
            	}
				throughput = hit.material.color.xyz * hit.material.color.w;
			}
			else if (met < 1.0)
			{
				hitRay.direction = hit.normal + randomDir;
				hitRay.direction = mix(hitRay.direction, reflectedDir, met);

				if (met == 1.0)
				{
					throughput = hit.material.color.xyz * hit.material.color.w;
				}
				else
				{
					float cosTheta = dot(hitRay.direction, hit.normal);
					float p = 1.0 / (2.0 * PI);
					vec3 BRDF = (hit.material.color.xyz * hit.material.color.w) / PI;
					throughput = (BRDF * throughput * cosTheta / p);
				}

			}
			else if (met >= 1.0)
			{
				
				hitRay.direction = reflectedDir;
				if (dot(hitRay.direction, hit.normal) <= 0.0)
				{
					break;
				}
				throughput *= hit.material.color.xyz * hit.material.color.w;
			}

			//hitRay.direction = hit.normal + randomDir;
			//throughput *= hit.material.color.xyz;
			tMax = hit.t;
			seed *= 1.456;
		}
		else
		{
			float t = 0.5 * (hitRay.direction.y + 1.0);
			vec3 background = ((1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0));
			//vec3 background = vec3(0.0);
			n++;
			color += throughput * background;
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

	int iterator = 0;
	float seed = 1.0;

	for (int i = 0; i < 4; ++i)
	{
		float rx = -0.5 + (0.5 + 0.5) * trand(uv, 0.123 * seed);
		float ry = -0.5 + (0.5 + 0.5) * trand(uv, 0.456 * seed);

    	ray.direction = GetRayDirection(ray.origin, fragCoord, rx, ry);
		color += TracePath(ray, uv, seed, iterator);
	}

	color = sqrt(color / float(iterator));

	// Gamma correction
	color = pow(color, vec3(1.0 / 2.2));

	color = mix(prev, color, 1.0 / float(samples + 1));

    fragColor = vec4(color, 1.0);
}