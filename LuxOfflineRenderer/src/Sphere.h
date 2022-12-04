#pragma once

#include "Hittable.h"
#include <glm/glm/gtx/norm.hpp>

namespace Lux
{
	class Sphere : public Hittable
	{
	public:
		Sphere(const glm::vec3& c, float r, const std::shared_ptr<RayMaterial>& mat) : center(c), radius(r), material(mat)
		{

		}
		
		bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& hit) const override
		{
			glm::vec3 originMinusCenter = ray.origin - center;

			// The sqrt length is the same as the dot product of direction, and it's more efficient
			float a = glm::length2(ray.direction);
			float b = /* 2.0f * */ glm::dot(originMinusCenter, ray.direction);
			float c = glm::length2(originMinusCenter) - (radius * radius);
			float discriminant = (b * b) - (a * c);

			if (discriminant < 0) return false;
			
			float sqrtd = sqrt(discriminant);

			// Find the nearest root that lies in the acceptable range.
			auto root = (-b - sqrtd) / a;
			if (root < tMin || tMax < root) 
			{
				root = (-b + sqrtd) / a;
				if (root < tMin || tMax < root)
					return false;
			}

			hit.t = root;
			hit.point = ray.at(hit.t);
			hit.normal = (hit.point - center) / radius;
			hit.material = material;

			hit.SetFaceNormal(ray, hit.normal);

			return true;
		}

	private:
		glm::vec3 center;
		float radius;

		std::shared_ptr<RayMaterial> material;
	};
}