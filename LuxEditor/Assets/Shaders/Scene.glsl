struct Sphere
{
	vec3 center;
	float radius;
};

struct Scene
{
	Sphere spheres[2];
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
};

vec3 GetRayAt(const Ray ray, float t)
{
	return ray.origin + t * ray.direction;
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

	return true;
}

bool IterateWorld(const Scene scene, const Ray ray, float tMin, float tMax, HitRecord hit)
{
	HitRecord tempHit;
	bool somethingHit;
	float closest = tMax;

	for (int i = 0; i < 2; ++i)
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