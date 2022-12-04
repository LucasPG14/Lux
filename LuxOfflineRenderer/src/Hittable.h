#pragma once

#include <vector>
#include <memory>
#include "Ray.h"

namespace Lux
{
	class RayMaterial;

	struct HitRecord
	{
		glm::vec3 point;
		glm::vec3 normal;
		std::shared_ptr<RayMaterial> material;
		float t;
		bool frontFace;

		inline void SetFaceNormal(const Ray& ray, glm::vec3 outNormal)
		{
			frontFace = glm::dot(ray.direction, outNormal) < 0;
			normal = frontFace ? outNormal : -outNormal;
		}
	};

	class Hittable
	{
	public:
		virtual bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& hit) const = 0;
	};

	class HittableList : public Hittable
	{
	public:
		HittableList() {}
		~HittableList() { objects.clear(); }

		void Add(std::shared_ptr<Hittable> object) { objects.push_back(object); }

		bool Hit(const Ray& ray, float tMin, float tMax, HitRecord& hit) const override
		{
			HitRecord tempHit = {};

			bool somethingHitted = false;
			float closest = tMax;

			for (const auto& object : objects)
			{
				if (object->Hit(ray, tMin, closest, tempHit))
				{
					somethingHitted = true;
					closest = tempHit.t;
					hit = tempHit;
				}
			}

			return somethingHitted;
		}

	private:
		std::vector<std::shared_ptr<Hittable>> objects;
	};
}