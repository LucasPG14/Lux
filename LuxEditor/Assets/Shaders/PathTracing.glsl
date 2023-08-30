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

	int transformID;
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

struct DirLight
{
	vec3 direction;
	vec3 radiance;
};
uniform DirLight dirLight;

struct PointLight
{
	vec3 position;
	vec3 radiance;
	float radius;
};
uniform PointLight pointLights[20];

struct LightHit
{
	vec3 point;
	vec3 normal;
	float t;
	bool frontFace;

	PointLight light;
};

uniform vec3 viewPos;
uniform vec2 canvas;
uniform mat4 inverseCamera;

uniform int samples;
uniform int numPointLights;

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
    return fract(sin(dot(n.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float hash(float n) { return fract(sin(n) * 1e4); }

float trand(vec2 n, float seed)
{
    return nrand(n * seed * float(samples));
}

vec2 trand2(vec2 n, float seed)
{
    return vec2(nrand(n * seed * float(samples)), nrand(n * hash(seed) * float(samples)));
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

bool Hit(PointLight light, const Ray ray, float tMin, float tMax, inout LightHit hit)
{
	vec3 oc = ray.origin - light.position;
    float a = dot(ray.direction, ray.direction);
    float b = dot(oc, ray.direction);
    float c = dot(oc, oc) - (light.radius * light.radius);
    float discriminant = (b * b) - (a * c);
    
	if (discriminant < 0.0)
		return false;

	float sqrtd = sqrt(discriminant);

	float root = (-b - sqrtd) / a;
    if (root < tMin || tMax < root) 
	{
        root = (-b + sqrtd) / a;
        if (root < tMin || tMax < root)
            return false;
    }

	hit.t = root;
	hit.point = GetRayAt(ray, root);
	hit.normal = (hit.point - light.position) / light.radius;
	hit.frontFace = dot(ray.direction, hit.normal) < 0.0;
	
	hit.light = light;

	return true;
}

bool HitAABB(vec3 origin, vec3 direction, int aabbID, float t)
{
	AABB aabb = aabbs[aabbID];

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
		return t0 < t;
	
	return t0 < t && t1 < t;
}

float fresnel(vec3 normal, vec3 direction, float biasScale, float power)
{
	return biasScale + (1.0 - biasScale) * pow(1.0 - dot(-direction, normal), power);
}

bool RayTriangleHit(Ray ray, inout HitRecord hit, float minT, float maxT)
{
	bool somethingHit = false;
	float closest = maxT;

	for (int j = 0; j < numObjects; ++j)
	{
		vec4 object = objects[j];

		mat4 modelMatrix = transforms[int(object.x)];

		vec3 origin = vec3(inverse(modelMatrix) * vec4(ray.origin, 1.0));
		vec3 direction = vec3(inverse(modelMatrix) * vec4(ray.direction, 0.0));

		if (!HitAABB(origin, direction, int(object.y), closest))
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
				hit.transformID = int(object.x);
			}
		}
	}

	return somethingHit;
}

vec3 GetMaterialColorPointLight(vec3 materialColor, vec3 normal, vec3 viewDir, vec3 lightDir, vec3 lightColor, float attenuation)
{
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * lightColor;
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    float specularStrength = 1.0;
    vec3 reflectDir = reflect(lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
    
    return (ambient + diffuse) * (specular + materialColor);
}

vec3 GetMaterialColorDirLight(vec3 materialColor, vec3 normal, vec3 viewDir, vec3 lightDir, vec3 lightColor)
{
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * lightColor;
    
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    float specularStrength = 1.0;
    vec3 reflectDir = reflect(lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;
    
    return (ambient + diffuse) * (specular + materialColor);
}

vec3 CosineSampleHemisphere(vec3 n, vec2 uv, float seed)
{
    vec2 rnd = trand2(uv, seed);

    float a = PI * 2.0 * rnd.x;
    float b = 2.0 * rnd.y - 1.0;
    
    vec3 dir = vec3(sqrt(1.0 - b * b) * vec2(cos(a), sin(a)), b);
    return normalize(n + dir);
}

vec3 SampleGGXVNDF(vec3 v, float ax, float ay, float r1, float r2)
{
    vec3 vh = normalize(vec3(ax * v.x, ay * v.y, v.z));

    float lensq = vh.x * vh.x + vh.y * vh.y;
    vec3 T1 = lensq > 0.0 ? vec3(-vh.y, vh.x, 0.0) * inversesqrt(lensq) : vec3(1.0, 0.0, 0.0);
    vec3 T2 = cross(vh, T1);

    float r = sqrt(r1);
    float phi = 2.0 * PI * r2;
    float t1 = r * cos(phi);
    float t2 = r * sin(phi);
    float s = 0.5 * (1.0 + vh.z);
    t2 = (1.0 - s) * sqrt(1.0 - t1 * t1) + s * t2;

    vec3 nh = t1 * T1 + t2 * T2 + sqrt(max(0.0, 1.0 - t1 * t1 - t2 * t2)) * vh;

    return normalize(vec3(ax * nh.x, ay * nh.y, max(0.0, nh.z)));
}

void Basis(in vec3 n, out vec3 b1, out vec3 b2) 
{
    if (n.z < 0.0)
	{
        float a = 1.0 / (1.0 - n.z);
        float b = n.x * n.y * a;
        b1 = vec3(1.0 - n.x * n.x * a, -b, n.x);
        b2 = vec3(b, n.y * n.y*a - 1.0, -n.y);
    }
    else
	{
        float a = 1.0 / (1.0 + n.z);
        float b = -n.x * n.y * a;
        b1 = vec3(1.0 - n.x * n.x * a, b, -n.x);
        b2 = vec3(b, 1.0 - n.y * n.y * a, -n.y);
    }
}

vec3 ToWorld(vec3 x, vec3 y, vec3 z, vec3 v)
{
    return v.x*x + v.y*y + v.z*z;
}

vec3 ToLocal(vec3 x, vec3 y, vec3 z, vec3 v)
{
    return vec3(dot(v, x), dot(v, y), dot(v, z));
}

vec3 Schlick(vec3 f0, float theta) 
{
    return f0 + (1.0 - f0) * pow(1.0 - theta, 5.0);
}

float Schlick(float f0, float f90, float theta) 
{
    return f0 + (f90 - f0) * pow(1.0 - theta, 5.0);
}

float Fresnel(float n1, float n2, float VoH, float f0, float f90)
{
    float r0 = (n1 - n2) / (n1 + n2);
    r0 *= r0;
    if (n1 > n2)
    {
        float n = n1 / n2;
        float sinT2 = n * n * (1.0 - VoH * VoH);
        if (sinT2 > 1.0)
            return f90;
        VoH = sqrt(1.0 - sinT2);
    }
    float x = 1.0 - VoH;
    float ret = r0 + (1.0 - r0)*pow(x, 5.0);
    
    return mix(f0, f90, ret);
}

vec3 DisneyDiffuseBRDF(vec3 color, float NoR, float NoD, float RoDR, float roughness) 
{
    float FD90 = 0.5 + 2.0 * roughness * pow(RoDR, 2.0);
    float a = Schlick(1.0, FD90, NoR);
    float b = Schlick(1.0, FD90, NoD);
    
    return color * (a * b / PI);
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
			hitRay.origin = hit.point + (hit.normal * 0.001);

			vec3 randomDir = CosineSampleHemisphere(hit.normal, uv, seed);
			//vec3 randomDir = hit.normal;

			vec3 newDir;

			// Material properties
			vec3 albedoColor = hit.material.color.xyz;
			float metallic = hit.material.properties.x;
			float roughness = hit.material.properties.y;
			float refractionIndex = hit.material.properties.z;
			float transmission = hit.material.properties.w;

			vec3 diffuseRay;
			vec3 specularRay;

			// Calculate microfacet
			vec3 t;
			vec3 b;

    		Basis(hit.normal, t, b);
    		vec3 v = ToLocal(t, b, hit.normal, -hitRay.direction);
    		vec3 h = SampleGGXVNDF(v, roughness, roughness, nrand(uv), hash(seed));
    		if (h.z < 0.0)
        		h = -h;
    		h = ToWorld(t, b, hit.normal, h);

			// Fresnel
			float VoH = dot(-hitRay.direction, h);
    		vec3 f0 = mix(vec3(0.04), albedoColor, metallic);
    		vec3 F = Schlick(f0, VoH);
			float dielectricF = Fresnel(1.0, refractionIndex, abs(VoH), 0.0, 1.0);

			float diffuseWeight = (1.0 - transmission) * (1.0 - metallic);
			float reflectWeight = metallic;
			float transmissionWeight = (1.0 - metallic) * transmission * (1.0 - dielectricF);

			float invW = 1.0 / (diffuseWeight + reflectWeight + transmissionWeight);

			diffuseWeight *= invW;
			reflectWeight *= invW;
			transmissionWeight *= invW;

			n++;
			if (diffuseWeight > 0.0)
			{
				float NoR = dot(hit.normal, randomDir);
				float NoD = dot(hit.normal, -hitRay.direction);
				if ( NoR <= 0.0 || NoD <= 0.0) 
				{ 
					newDir = randomDir;
					hitRay.direction = newDir;
				}
				float RoDR = dot(randomDir, normalize(randomDir + hitRay.direction));
        		float pdf = NoR / PI;

				vec3 diffuse = DisneyDiffuseBRDF(albedoColor, NoR, NoD, RoDR, roughness) * (1.0 - F);
        		
        		float finalPDF = diffuseWeight * pdf;

				if (finalPDF > 0.0)
				{
					throughput *= ((diffuse * abs(dot(hit.normal, randomDir))) / pdf);
					for (int i = 0; i < numPointLights; ++i)
					{
						vec3 L = normalize(pointLights[i].position - vec3(transforms[hit.transformID] * vec4(hit.point, 1.0)));
						float NdotL = max(dot(hit.normal, L), 0.0);
						if (NdotL > 0.0)
							throughput *= (pointLights[i].radiance * NdotL);
					}
				}
			}

			newDir = randomDir;

			hitRay.direction = newDir;
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

	float rx = -0.5 + (0.5 + 0.5) * trand(uv, 0.123 * seed);
	float ry = -0.5 + (0.5 + 0.5) * trand(uv, 0.456 * seed);

    ray.direction = GetRayDirection(ray.origin, fragCoord, rx, ry);
	color = TracePath(ray, uv, seed, iterator);

	color = sqrt(color / float(iterator));

	// Gamma correction
	color = pow(color, vec3(1.0 / 2.2));

	color = mix(prev, color, 1.0 / float(samples + 1));

    fragColor = vec4(color, 1.0);
}