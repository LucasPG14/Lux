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

struct Mat
{
	vec3 albedo;
	float metallic;
};

struct Sphere
{
	vec3 center;
	float radius;
	Mat mat;
};

struct Scene
{
	Sphere spheres[4];
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
	bool frontFace;

	Mat material;
};

bool ScatterMetallic(const Ray inRay, const HitRecord hit, inout vec3 attenuation, inout Ray scatteredRay)
{
	scatteredRay.origin = hit.point;
	scatteredRay.direction = reflect(normalize(inRay.direction), hit.normal);
	attenuation = hit.material.albedo;

	return (dot(scatteredRay.direction, hit.normal) > 0.0);
}

float Rand(vec2 coord)
{
	return fract(sin(dot(coord, vec2(12.9898, 78.233))) * 43758.5453);
}

bool ScatterAlbedo(const Ray inRay, const HitRecord hit, inout vec3 attenuation, inout Ray scatteredRay)
{
	scatteredRay.direction = hit.normal + vec3(Rand(hit.point.xy), Rand(hit.point.xz), Rand(hit.point.yz));

	attenuation = hit.material.albedo;
	return true;
}

vec3 GetRandomRange(float fMin, float fMax, vec2 value)
{
	vec3 randomVec = vec3(Rand(value), Rand(value + 3000.0), Rand(value + 6000.0));
	//randomVec = min(randomVec, fMax);
	//randomVec = max(randomVec, fMin);
	return noise3(randomVec);
}

vec3 RandomUnitSphere()
{
	vec2 frag = gl_FragCoord.xy;
	int i = 0;
	while(true)
	{
		vec3 point = 2.0 * GetRandomRange(-1.0, 1.0, frag + float(i));
		i += 9000;
		//point = mix(vec3(-1.0), vec3(1.0), point);
		if (dot(point, point) > 1.0) 
			continue;

		return point;
	}
}

vec3 RandomHemisphere(vec3 normal)
{
	vec3 dir = RandomUnitSphere();
	
	if (dot(dir, normal) > 0.0)
		return dir;

	return -dir;
}

vec3 GetRayAt(const Ray ray, float t)
{
	return ray.origin + (t * ray.direction);
}

bool Hit(Sphere sphere, const Ray ray, float tMin, float tMax, inout HitRecord hit)
{
	vec3 oc = ray.origin - sphere.center;
    float a = dot(ray.direction, ray.direction);
    float b = dot(oc, ray.direction);
    float c = dot(oc, oc) - (sphere.radius * sphere.radius);
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
	vec3 normal = (hit.point - sphere.center) / sphere.radius;
	hit.frontFace = dot(ray.direction, normal) < 0.0;
	hit.normal = hit.frontFace ? normal : -normal;
	hit.material = sphere.mat;

	return true;
}

bool IterateWorld(const Scene scene, const Ray ray, float tMin, float tMax, inout HitRecord hit)
{
	HitRecord tempHit;
	bool somethingHit = false;
	float closest = tMax;

	for (int i = 0; i < 4; ++i)
	{
		if (Hit(scene.spheres[i], ray, tMin, closest, tempHit))
		{
			somethingHit = true;
			closest = tempHit.t; 
			hit = tempHit; 
		}
	}

	return somethingHit;
}

struct AABB
{
	vec3 min;
	vec3 max;
};
uniform AABB aabbs[100];

out vec4 fragColor;

layout(location = 0) uniform sampler2D positions;
layout(location = 1) uniform sampler2D normals;
layout(location = 2) uniform sampler2D albedoSpecular;

in vec2 texCoord;

uniform vec3 viewPos;
uniform vec2 canvas;
uniform vec2 viewportSize;

uniform mat4 cameraMatrix;
uniform mat4 modelMatrix;

uniform vec3 right;
uniform vec3 front;
uniform vec3 up;

uniform float nearPlane;
uniform float farPlane;

uniform float aspectRatio;

uniform float horizontalFov;
uniform float verticalFov;

uniform float fov;

const float maxFloat = 10000000.0;

float Intersection(Ray ray)
{
	for (int i = 0; i < 100; ++i)
	{
		AABB aabb = aabbs[i];

		aabb.min = vec3(modelMatrix * vec4(aabb.min, 1.0));
		aabb.max = vec3(modelMatrix * vec4(aabb.max, 1.0));

		ray.origin = mat3(inverse(modelMatrix)) * ray.origin;
		ray.direction = mat3(inverse(modelMatrix)) * ray.direction;

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

vec3 GetRayDirection(vec3 origin, float rx, float ry)
{
	vec3 horizontal = vec3(viewportSize.x, 0.0, 0.0);
	vec3 vertical = vec3(0.0, viewportSize.y, 0.0);
	vec3 lowerLeftCorner = origin - (horizontal / 2) - (vertical / 2) - vec3(0.0, 0.0, 1.0);

	float u = (gl_FragCoord.x + rx) / (canvas.x - 1);
	float v = (gl_FragCoord.y + ry) / (canvas.y - 1);

	return lowerLeftCorner + (u * horizontal) + (v * vertical) - origin;
}

float HitSphere(const vec3 center, float radius, const Ray ray) 
{
    vec3 oc = ray.origin - center;
    float a = dot(ray.direction, ray.direction);
    float b = 2.0 * dot(oc, ray.direction);
    float c = dot(oc, oc) - (radius * radius);
    float discriminant = (b * b) - (4.0 * a * c);
    
	if (discriminant < 0.0)
	{
		return -1.0;
	}

	return (-b - sqrt(discriminant) ) / (2.0 * a);
}

vec3 TracePath(Ray ray)
{
//	if (HitSphere(vec3(0.0, 0.0, -1.0), 1.0, ray))
//		return vec3(1.0, 0.0, 0.0);

//	float t = Intersection(ray);
//	if (t > 0.0)
//	{
//		vec3 normal = normalize(GetRayAt(ray, t) - vec3(0, 0, -1));
//		return 0.5 * vec3(normal.x + 1.0, normal.y + 1.0, normal.z + 1.0);
//
//		//vec3 point = GetRayAt(ray, t);
//		//return vec3(1.0, 0.0, 0.0);
//	}	

	HitRecord hit;
	Scene scene;
	scene.spheres[0].center = vec3(0.0, 0.0, -1.0);
	scene.spheres[0].radius = 0.5;
	scene.spheres[0].mat.albedo = vec3(0.7, 0.3, 0.3);
	scene.spheres[0].mat.metallic = 0.0;

	scene.spheres[1].center = vec3(0.0, -100.5, -1.0);
	scene.spheres[1].radius = 100.0;
	scene.spheres[1].mat.albedo = vec3(0.8, 0.8, 0.0);
	scene.spheres[1].mat.metallic = 0.0;

	scene.spheres[2].center = vec3(-1.0, 0.0, -1.0);
	scene.spheres[2].radius = 0.5;
	scene.spheres[2].mat.albedo = vec3(0.8, 0.8, 0.8);
	scene.spheres[2].mat.metallic = 1.0;

	scene.spheres[3].center = vec3(1.0, 0.0, -1.0);
	scene.spheres[3].radius = 0.5;
	scene.spheres[3].mat.albedo = vec3(0.8, 0.6, 0.2);
	scene.spheres[3].mat.metallic = 1.0;
	//float t = HitSphere(vec3(0.0, 0.0, -1.0), 1.0, ray);
	Ray bounceRay = ray;
	vec3 attenuation = vec3(1.0);
	vec3 att = vec3(1.0);
	
	for (int i = 0; i < 50; ++i)
	{
		if (IterateWorld(scene, bounceRay, 0.001, maxFloat, hit))
		{
			Ray scatterRay;
			if (hit.material.metallic > 0.0)
			{
				if (ScatterMetallic(bounceRay, hit, att, scatterRay))
				{
					attenuation *= att;
					bounceRay = scatterRay;
				}
				else
				{
					return vec3(0.0);
				}
			}
			else
			{
				if (ScatterAlbedo(bounceRay, hit, att, scatterRay))
				{
					attenuation *= att;
					bounceRay = scatterRay;
				}
				else
				{
					return vec3(0.0);
				}
			}

			vec3 target = hit.point + hit.normal + RandomUnitSphere();

			bounceRay.origin = hit.point;
			bounceRay.direction = target - hit.point;
			attenuation *= 0.5;
		}
		else
		{
			float t = 0.5 * (normalize(bounceRay.direction).y + 1.0);
			vec3 backColor = ((1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0));
			return attenuation * backColor;
		}
	}

	return vec3(0.0);
}

Ray GetRayDir()
{
//	float x =  gl_FragCoord.x * 2.0 - 1.0;
//	float y =  gl_FragCoord.y * 2.0 - 1.0;
	
	// Near Plane Pos
//	float halfWidthNearPlane = tan(horizontalFov * 0.5) * nearPlane;
//	float halfHeightNearPlane = tan(verticalFov * 0.5) * nearPlane;
//
//	float nearX = x * halfWidthNearPlane;
//	float nearY = y * halfHeightNearPlane;
//
//	vec3 nearPlaneVec = viewPos + front * nearPlane + nearX * right + nearY * up;

	// Far Plane Pos
//	float halfWidthFarPlane = tan(horizontalFov * 0.5) * farPlane;
//	float halfHeightFarPlane = tan(verticalFov * 0.5) * farPlane;
//
//	float farX = x * halfWidthFarPlane;
//	float farY = y * halfHeightFarPlane;
//
//	vec3 farPlaneVec = viewPos + front * farPlane + farX * right + farY * up;
//
//	Ray ray;
//	ray.origin = viewPos;
//	ray.direction = normalize(farPlaneVec - nearPlaneVec);
//	return ray;
	//return Ray(nearPlaneVec, normalize(farPlaneVec - nearPlaneVec));

	//float x = (gl_FragCoord.x / canvas.x) * 2.0 - 1.0;
	//float y = (gl_FragCoord.y / canvas.y) * 2.0 - 1.0;

	float pixelX = (2.0 * ((gl_FragCoord.x + 0.5) / canvas.x) - 1.0) * tan(fov / 2.0 * 3.14 / 180.0) * aspectRatio;
	float pixelY = (1.0 - 2.0 * ((gl_FragCoord.y + 0.5) / canvas.y) * tan(fov / 2.0 * 3.14 / 180.0));

	vec3 direction = mat3(cameraMatrix) * vec3(pixelX, pixelY, 1.0);
	Ray ray;
	ray.origin = viewPos;
	ray.direction = normalize(direction - ray.origin);

	return ray;
}

void main()
{
	vec3 position = texture(positions, texCoord).rgb;
	vec3 normals = texture(normals, texCoord).rgb;
	vec3 albedo = texture(albedoSpecular, texCoord).rgb;

	Ray ray;
	ray.origin = viewPos;
	//ray.direction = vec3(cameraMatrix * vec4(GetRayDirection(ray.origin), 1.0));

	//Ray ray = GetRayDir();
	//ray.origin = vec3(cameraMatrix * vec4(ray.origin, 1.0));
	//ray.direction = vec3(cameraMatrix * vec4(ray.direction, 1.0));
	vec3 color = vec3(0.0);

	vec2 frag = gl_FragCoord.xy;

	for (int i = 0; i < 100; ++i)
	{
		float rx = Rand(vec2(frag.x + float(i), frag.y + float(i)));
		float ry = Rand(vec2(frag.y + float(i), frag.x + float(i)));
		
		ray.direction = GetRayDirection(ray.origin, rx, ry);
		color += TracePath(ray);
	}

	float scale = 1.0 / 100.0;
	color *= scale;
	//color = sqrt(scale * color);

	fragColor = vec4(color, 1.0);
}