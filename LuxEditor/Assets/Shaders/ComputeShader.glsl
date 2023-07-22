#version 450 core

layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D imgOutput;

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

// Uniforms
uniform vec3 viewPos;
uniform vec2 canvas;
uniform mat4 inverseCamera;

uniform int samples;

layout(location = 1) uniform sampler2D accumulateTexture;
layout(location = 4) uniform sampler2D transformsTex;
layout(location = 8) uniform sampler2D normalsTex;


//SSBOs
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

	for (int j = 0; j < 6; ++j)
	{
		vec4 object = objects[j];

		vec4 r1 = texelFetch(transformsTex, ivec2(object.w * 4, 0), 0).xyzw;
		vec4 r2 = texelFetch(transformsTex, ivec2(object.w * 4 + 1, 0), 0).xyzw;
		vec4 r3 = texelFetch(transformsTex, ivec2(object.w * 4 + 2, 0), 0).xyzw;
		vec4 r4 = texelFetch(transformsTex, ivec2(object.w * 4 + 3, 0), 0).xyzw;

		mat4 modelMatrix = mat4(r1, r2, r3, r4);

		vec3 origin = ray.origin;
		vec3 direction = ray.direction;

		for (int i = 0; i < 24; ++i)
		{
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

			if (t > 0.0 && minT < t && t < closest)
			{
				closest = t;
				somethingHit = true;
				hit.t = t;

				hit.point = GetRayAt(ray, t);

				vec2 tC1 = vec2(vertices[int(indices[i].x)].w, texelFetch(normalsTex, ivec2(int(indices[i].x), 0), 0).w);
				vec2 tC2 = vec2(vertices[int(indices[i].y)].w, texelFetch(normalsTex, ivec2(int(indices[i].y), 0), 0).w);
				vec2 tC3 = vec2(vertices[int(indices[i].z)].w, texelFetch(normalsTex, ivec2(int(indices[i].z), 0), 0).w);

				vec2 texCoords = tC1 * (1.0 - u - v) + tC2 * u + tC3 * v;
				hit.texCoords = texCoords;

				hit.material.color = object.xyz;

				vec3 normal1 = texelFetch(normalsTex, ivec2(int(indices[i].x), 0), 0).xyz;
				vec3 normal2 = texelFetch(normalsTex, ivec2(int(indices[i].y), 0), 0).xyz;
				vec3 normal3 = texelFetch(normalsTex, ivec2(int(indices[i].z), 0), 0).xyz;

				hit.normal = normalize(normal1 * (1.0 - u -v) + normal2 * u + normal3 * v);
				hit.normal = normalize(transpose(inverse(mat3(modelMatrix))) * hit.normal);
				
				hit.normal = dot(ray.direction, hit.normal) < 0.0 ? -hit.normal : hit.normal;
			}
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
	for (; i < 2; ++i)
	{
		if (RayTriangleHit(hitRay, hit, 0.001, tMax))
		{
			hitRay.origin = hit.point;
			//hitRay.direction = reflect(hitRay.direction, hit.normal);
			hitRay.direction = normalize(hit.normal + randomPointInUnitSphere(uv, newSeed));
			tMax = hit.t;
			color *= (hit.material.color * 0.5);
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

	ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

	vec2 fragCoord;
	fragCoord.x = float(texelCoord.x)/(gl_NumWorkGroups.x);
    fragCoord.y = float(texelCoord.y)/(gl_NumWorkGroups.y);
	//fragCoord.x = float(texelCoord.x / 897.0) * 2.0 - 1.0;
	//fragCoord.y = float(texelCoord.y / 701.0) * 2.0 - 1.0;

	vec2 uv = texelCoord / canvas;
	vec3 color;

	float rx = trand(uv, 0.123);
	float ry = trand(uv, 0.456);

	vec3 prev = texelFetch(accumulateTexture, texelCoord, 0).rgb;
	
	ray.direction = GetRayDirection(ray.origin, texelCoord, rx, ry);

	color = TracePath(ray, uv);

	color = pow(color, vec3(1.0 / 2.2));
	
	if (prev != vec3(0.0, 0.0, 0.0))
	{
		color = (float(samples - 1) * prev + color) / float(samples);
	}
	
    imageStore(imgOutput, texelCoord, vec4(color, 1.0));
}