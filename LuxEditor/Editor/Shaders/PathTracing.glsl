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

struct MaterialInfo
{
	vec4 textureIDs;
	vec4 color;
	vec4 properties;
	vec4 emissive;
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

struct RefractionState
{
	bool isRefracted;
	bool wasRefracted;
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
	float intensity;
};
uniform DirLight dirLight;

struct PointLight
{
	vec3 position;
	vec3 radiance;
	float intensity;
};
uniform PointLight pointLights[100];

uniform vec3 viewPos;
uniform vec2 canvas;
uniform mat4 inverseCamera;

uniform int samples;
uniform int maxBounces;
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

uniform int seed3;
int seed2 = 1;
int rand() 
{ 
	seed2 = seed2 * 0x343fd+0x269ec3; 
	return (seed2 >> 16)&32767; 
}

float frand() 
{ 
	return float(rand()) / 32767.0; 
}
vec2 frand2() 
{ 
	return vec2(frand(), frand()); 
}

float hash(float n) { return fract(sin(n) * 1e4); }

float nrand(vec2 n)
{
    return fract(sin(dot(n.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

float trand(vec2 n, float seed)
{
    return nrand(n * seed * float(samples));
}

void srand(ivec2 p, int frameCount)
{
    int n = frameCount;
    n = (n << 13) ^ n; 
	n = n * (n * n * 15731 + 789221) + 1376312589;
    n += p.y;
    n = (n << 13) ^ n; 
	n = n * (n * n * 15731 + 789221) + 1376312589;
    n += p.x;
    n = (n << 13) ^ n; 
	n = n * (n * n * 15731 + 789221) + 1376312589;
    seed2 = n;
}

vec3 GetRayAt(const Ray ray, float t)
{
	return ray.origin + t * ray.direction;
}

float HitAABB(vec3 origin, vec3 direction, int aabbID, float t)
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
		return -1.0;
	
	if (t0 > 0.0f)
		return t0;
	
	return t1;
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

		float aabbT = HitAABB(origin, direction, int(object.y), closest);
		if (aabbT == -1.0 || aabbT > closest)
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

				hit.normal = normalize(normal1.xyz * (1.0 - u - v) + normal2.xyz * u + normal3.xyz * v);
				hit.normal = normalize(transpose(inverse(mat3(modelMatrix))) * hit.normal);

				hit.frontFace = dot(hit.normal, ray.direction) <= 0.0;
				hit.normal = hit.frontFace ? hit.normal : -hit.normal;
				hit.transformID = int(object.x);
			}
		}
	}

	return somethingHit;
}

vec3 cosineSampleHemisphere(vec3 n)
{
    vec2 rnd = frand2();

	rnd.x = max(min(rnd.x, 1.0), 0.0);
	rnd.y = max(min(rnd.y, 1.0), 0.0);

    float a = PI * 2.0 * rnd.y;
    float b = 2.0 * rnd.x - 1.0;
    
    vec3 dir = vec3(sqrt(1.0 - b * b) * vec2(cos(a), sin(a)), b);
    return normalize(n + dir);
}

void TangentAndBitangent(in vec3 n, out vec3 b1, out vec3 b2) 
{
    if(n.z < 0.0)
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

vec3 toWorld(vec3 x, vec3 y, vec3 z, vec3 v)
{
    return v.x * x + v.y * y + v.z * z;
}

vec3 toLocal(vec3 x, vec3 y, vec3 z, vec3 v)
{
    return vec3(dot(v, x), dot(v, y), dot(v, z));
}

float Fresnel(float n1, float n2, float VdotH, float f0, float f90)
{
    float r0 = (n1 - n2) / (n1 + n2);
    r0 *= r0;
    if (n1 > n2)
    {
        float n = n1/n2;
        float sinT2 = n * n * (1.0 - VdotH * VdotH);
        if (sinT2 > 1.0)
            return f90;
        VdotH = sqrt(1.0 - sinT2);
    }
    float x = 1.0 - VdotH;
    float ret = r0 + (1.0 - r0) * pow(x, 5.0);
    
    return mix(f0, f90, ret);
}

vec3 FSchlick(vec3 f0, float theta) 
{
    return f0 + (1.0 - f0) * pow(1.0 - theta, 5.0);
}

float FSchlick(float f0, float f90, float theta) 
{
    return f0 + (f90 - f0) * pow(1.0 - theta, 5.0);
}

float DGTR(float roughness, float NdotH, float k) 
{
    float a2 = pow(roughness, 2.0);
    return a2 / (PI * pow((NdotH * NdotH) * (a2 * a2 - 1.0) + 1.0, k));
}

float SmithG(float NdotV, float roughness2)
{
    float a = pow(roughness2, 2.0);
    float b = pow(NdotV, 2.0);
    return (2.0 * NdotV) / (NdotV + sqrt(a + b - a * b));
}

float GeometryTerm(float NdotL, float NdotV, float roughness)
{
    float a2 = roughness * roughness;
    float G1 = SmithG(NdotV, a2);
    float G2 = SmithG(NdotL, a2);
    return G1 * G2;
}

vec3 SampleGGXVNDF(vec3 V, float ax, float ay, float r1, float r2)
{
    vec3 Vh = normalize(vec3(ax * V.x, ay * V.y, V.z));

    float lensq = Vh.x * Vh.x + Vh.y * Vh.y;
    vec3 T1 = lensq > 0.0 ? vec3(-Vh.y, Vh.x, 0.0) * inversesqrt(lensq) : vec3(1.0, 0.0, 0.0);
    vec3 T2 = cross(Vh, T1);

    float r = sqrt(r1);
    float phi = 2.0 * PI * r2;
    float t1 = r * cos(phi);
    float t2 = r * sin(phi);
    float s = 0.5 * (1.0 + Vh.z);
    t2 = (1.0 - s) * sqrt(1.0 - t1 * t1) + s * t2;

    vec3 Nh = t1 * T1 + t2 * T2 + sqrt(max(0.0, 1.0 - t1 * t1 - t2 * t2)) * Vh;

    return normalize(vec3(ax * Nh.x, ay * Nh.y, max(0.0, Nh.z)));
}

float GGXVNDFPdf(float NdotH, float NdotV, float roughness)
{
 	float D = DGTR(roughness, NdotH, 2.0);
    float G1 = SmithG(NdotV, roughness * roughness);
    return (D * G1) / max(0.00001, 4.0f * NdotV);
}

vec3 DisneyDiffuseBRDF(vec3 color, float NdotL, float NdotV, float LdotH, float roughness) 
{
    float FD90 = 0.5 + 2.0 * roughness * pow(LdotH, 2.0);
    float a = FSchlick(1.0, FD90, NdotL);
    float b = FSchlick(1.0, FD90, NdotV);
    
    return color * (a * b / PI);
}

vec3 DisneyReflectionBRDF(float matRoughness, vec3 F, float NdotH, float NdotV, float NdotL) 
{
    float roughness = pow(matRoughness, 2.0);
    float D = DGTR(roughness, NdotH, 2.0);
    float G = GeometryTerm(NdotL, NdotV, pow(0.5 + matRoughness * 0.5, 2.0));

    vec3 spec = D * F * G / (4.0 * NdotL * NdotV);
    
    return spec;
}

vec4 DisneyRefractionBRDF(vec3 albedo, float F, float NdotH, float NdotV, float NdotL, float VdotH, float LdotH, float refractionRatio, float matRoughness) 
{
    float roughness = pow(matRoughness, 2.0);
    float D = DGTR(roughness, NdotH, 2.0);
    float G = GeometryTerm(NdotL, NdotV, pow(0.5 + matRoughness * 0.5, 2.0));
    float denom = pow(LdotH + VdotH * refractionRatio, 2.0);

    float jacobian = abs(LdotH) / denom;
    float pdf = SmithG(abs(NdotL), roughness * roughness) * max(0.0, VdotH) * D * jacobian / NdotV;
    
    vec3 spec = pow(1.0 - albedo, vec3(0.5))  * D * (1.0 - F) * G * abs(VdotH) * jacobian * pow(refractionRatio, 2.0) / abs(NdotL * NdotV);
    return vec4(spec, pdf);
}

void CalculateBRDF(vec3 direction, HitRecord hit, inout RefractionState refState, out vec3 L, out vec4 brdf)
{
	// Material properties
	vec3 albedo = hit.material.color.xyz;
	if (hit.material.textureIDs.x != -1) albedo *= texture(texturesTex, vec3(hit.texCoords, hit.material.textureIDs.x)).xyz;
	float metallic = hit.material.properties.x;
	if (hit.material.textureIDs.z != -1) metallic *= texture(texturesTex, vec3(hit.texCoords, hit.material.textureIDs.z)).x;
	float roughness = hit.material.properties.y;
	if (hit.material.textureIDs.w != -1) roughness *= texture(texturesTex, vec3(hit.texCoords, hit.material.textureIDs.w)).x;
	float refractionIdx = hit.material.properties.z;
	float transmission = hit.material.properties.w;

	float roughness2 = pow(roughness, 2.0);

    // Calculating Microfacet normal
    vec3 t, b;
    TangentAndBitangent(hit.normal, t, b);
	vec3 V = toLocal(t, b, hit.normal, direction);
    vec3 h = SampleGGXVNDF(V, roughness2, roughness2, frand(), frand());
    if (h.z < 0.0)
        h = -h;
	h = toWorld(t, b, hit.normal, h);

	//if (hit.material.textureIDs.y != -1)
	//{
	//	vec3 n = texture(texturesTex, vec3(hit.texCoords, hit.material.textureIDs.y)).xyz;
	//}

    // Fresnel
    float VdotH = dot(direction, h);
    vec3 f0 = mix(vec3(0.04), albedo, metallic);
    vec3 fresnelValue = FSchlick(f0, VdotH);
    float dielectricFresnel = Fresnel(hit.frontFace ? 1.0 : refractionIdx, refractionIdx, abs(VdotH), 0.0, 1.0);
    
    // Weight probability
    float diffuseWeight = (1.0 - metallic) * (1.0 - transmission);
    float reflectWeight = dot(fresnelValue, vec3(0.299, 0.587, 0.114));
    float refractWeight = (1.0 - metallic) * (transmission) * (1.0 - dielectricFresnel);
    float invW = 1.0 / (diffuseWeight + reflectWeight + refractWeight);
    
	diffuseWeight *= invW;
    reflectWeight *= invW;
    refractWeight *= invW;

	refState.wasRefracted = refState.isRefracted;

	float rnd = frand();
	if (rnd < diffuseWeight)
	{
		L = cosineSampleHemisphere(hit.normal);
        h = L + direction;
        
        float NdotL = dot(hit.normal, L);
        float NdotV = dot(hit.normal, direction);
        if (NdotL <= 0.0 || NdotV <= 0.0) 
		{ 
			return; 
		}
        
		float LdotH = dot(L, h);
        float pdf = NdotL / PI;
        
        vec3 diffuse = DisneyDiffuseBRDF(albedo, NdotL, NdotV, LdotH, roughness2) * (1.0 - fresnelValue);
        brdf.rgb = diffuse;
        brdf.a = diffuseWeight * pdf;
	}
	else if (rnd < diffuseWeight + reflectWeight)
	{
		L = reflect(-direction, h);
        
        float NdotL = dot(hit.normal, L);
        float NdotV = dot(hit.normal, direction);
        if (NdotL <= 0.0 || NdotV <= 0.0) 
		{ 
			return; 
		}
        
		float NdotH = min(0.99, dot(hit.normal, h));
        float pdf = GGXVNDFPdf(NdotH, NdotV, roughness2);
        
        vec3 specular = DisneyReflectionBRDF(roughness, fresnelValue, NdotH, NdotV, NdotL);
        brdf.rgb = specular;
        brdf.a = reflectWeight * pdf;
	}
	else
	{
		refState.isRefracted = !refState.isRefracted;
		float refractionRatio = hit.frontFace ? 1.0 / refractionIdx : refractionIdx;
        L = refract(-direction, h, refractionRatio);
        
        float NdotL = dot(hit.normal, L);
        if (NdotL <= 0.0) 
		{ 
			return; 
		}
        
		float NdotV = dot(hit.normal, direction);
        float NdotH = min(0.99, dot(hit.normal, h));
        float LdotH = dot(L, h);
        
        brdf = DisneyRefractionBRDF(albedo, dielectricFresnel, NdotH, NdotV, NdotL, VdotH, LdotH, refractionRatio, roughness);
        brdf.a = refractWeight * brdf.a;
	}

	brdf.rgb *= abs(dot(hit.normal, L));
}

vec3 TracePath(const Ray ray, vec2 uv)
{
	vec3 color = vec3(0.0);
	Ray hitRay = ray;
	float tMax = 1000000.0;

	float accum = 1.0;

	vec3 throughput = vec3(1.0);

	RefractionState refState;
	refState.isRefracted = false;
	refState.wasRefracted = false;
	
	int i = 0;
	for (; i < maxBounces; ++i)
	{
		HitRecord hit;
		hitRay.direction = normalize(hitRay.direction);
		if (RayTriangleHit(hitRay, hit, 0.001, tMax))
		{
			vec3 newDir;
			vec4 brdf = vec4(0.0);

			CalculateBRDF(-hitRay.direction, hit, refState, newDir, brdf);

			if (hit.material.emissive.w == 1.0)
				color += hit.material.emissive.xyz * throughput;

			for (int i = 0; i < numPointLights; ++i)
			{
				vec3 L = normalize(pointLights[i].position - hit.point);
    			vec3 H = normalize(hitRay.direction + L);
				float distance = length(pointLights[i].position - hit.point);
			
    			float attenuation = 1.0 / distance;
    			vec3 radiance = pointLights[i].radiance * attenuation * pointLights[i].intensity;
			
				float NdotL = max(dot(hit.normal, L), 0.0);
				
				color += radiance * NdotL * throughput;
			}

			float directionLight = max(dot(hit.normal, normalize(-dirLight.direction)), 0.0);

			color += dirLight.radiance * directionLight * throughput * dirLight.intensity;

			if (brdf.a > 0.0)
				throughput *= brdf.rgb / brdf.a;

			if (refState.wasRefracted)
			{
				vec4 col = hit.material.color;
            	throughput *= exp(-hit.t * ((vec3(1.0) - col.xyz) * col.w));
        	}

			hitRay.origin = hit.point;

			if (refState.isRefracted) 
			{
            	hitRay.origin += -hit.normal * 0.01;
        	} 
			else if (refState.wasRefracted && !refState.isRefracted) 
			{
            	hitRay.origin += -hit.normal * 0.01;
        	} 
			else
			{
            	hitRay.origin += hit.normal * 0.01;
        	}

			hitRay.direction = newDir;

			float q = max(throughput.r, max(throughput.g, throughput.b));
            if (frand() > q)
                break;

            throughput /= q;

			if (i == maxBounces - 1)
			{
				color = throughput;
			}
		}
		else
		{
			float t = 0.5 * (hitRay.direction.y + 1.0);
			vec3 background = ((1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0));
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
	
	srand(ivec2(fragCoord), samples);

	float rx = -0.5 + (0.5 + 0.5) * trand(uv, 0.123 * frand());
	float ry = -0.5 + (0.5 + 0.5) * trand(uv, 0.456 * frand());

    ray.direction = GetRayDirection(ray.origin, fragCoord, rx, ry);
	color = TracePath(ray, uv);

	color = min(color.rgb, vec3(10.0));

	// Gamma correction and tonemapping
	//color = color / (color + vec3(1.0));
	//color = pow(color, vec3(1.0 / 2.2));

	color = (float(samples) * prev + color) / float(samples + 1);

    fragColor = vec4(color, 1.0);
}