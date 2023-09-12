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

struct Ray
{
    vec3 origin;
    vec3 direction;
};

struct HitRecord
{
	vec3 point;
	vec3 normal;
	float t;

	vec2 texCoords;

	MaterialInfo material;
};

uniform vec3 viewPos;
uniform vec2 canvas;
uniform mat4 inverseCamera;

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

layout(std430, binding = 4) buffer transformsSSBO
{
    vec4 meshes[];
};

layout(std430, binding = 5) buffer materialsSSBO
{
    MaterialInfo materials[];
};

layout(location = 4) uniform sampler2D transformsTex;

float nrand(vec2 n)
{
    return fract(sin(dot(n.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float trand(vec2 n, float seed)
{
    return nrand(n * seed);
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

		vec4 r1 = texelFetch(transformsTex, ivec2(object.x * 4, 0), 0).xyzw;
		vec4 r2 = texelFetch(transformsTex, ivec2(object.x * 4 + 1, 0), 0).xyzw;
		vec4 r3 = texelFetch(transformsTex, ivec2(object.x * 4 + 2, 0), 0).xyzw;
		vec4 r4 = texelFetch(transformsTex, ivec2(object.x * 4 + 3, 0), 0).xyzw;
		
		//vec4 r1 = transforms[j * 4];
		//vec4 r2 = transforms[j * 4 + 1];
		//vec4 r3 = transforms[j * 4 + 2];
		//vec4 r4 = transforms[j * 4 + 3];
		
		//mat4 modelMatrix = transforms[j];
		mat4 modelMatrix = mat4(r1, r2, r3, r4);

		vec3 origin = vec3(inverse(modelMatrix) * vec4(ray.origin, 1.0));
		vec3 direction = vec3(inverse(modelMatrix) * vec4(ray.direction, 0.0));

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

				hit.normal = dot(hit.normal, ray.direction) <= 0.0 ? hit.normal : -hit.normal;
			}
		}
	}

	return somethingHit;
}

vec3 TracePath(const Ray ray, vec2 uv)
{
	vec3 color = vec3(0.0);
	HitRecord hit;
	Ray hitRay = ray;
	float tMax = 1000000.0;

	float newSeed = 1.0;
	float accum = 1.0;

	vec3 attenuation = vec3(1.0);
	
	if (RayTriangleHit(hitRay, hit, 0.001, tMax))
	{
		hitRay.origin = hit.point;

		vec3 randomDir = randomPointInUnitSphere(uv, newSeed);
		if (hit.material.properties.x <= 0.0)
		{
			hitRay.direction = hit.normal + randomDir;
		}
		else
		{
			hitRay.direction = reflect(hitRay.direction, hit.normal) + randomDir;
		}

		tMax = hit.t;
		color += hit.material.color.xyz;
		attenuation *= 0.5;
		newSeed *= 1.456;
	}
	else
	{
		float t = 0.5 * (normalize(hitRay.direction).y + 1.0);
		color += ((1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0));
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

	vec2 fragCoord = gl_FragCoord.xy;
	vec2 uv = fragCoord / canvas;

	float rx = trand(uv, 0.123);
	float ry = trand(uv, 0.456);

    ray.direction = GetRayDirection(ray.origin, fragCoord, rx, ry);

	color = TracePath(ray, uv);

	// Gamma correction
	color = pow(color, vec3(1.0 / 2.2));

    fragColor = vec4(color, 1.0);
}