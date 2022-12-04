#pragma once

#include "Hittable.h"
#include "Utils.h"

namespace Lux
{
	class RayMaterial
	{
	public:
		virtual bool Scatter(const Ray& inRay, const HitRecord& hit, glm::vec3& attenuation, Ray& scatteredRay) const = 0;
	};

	bool IsZero(const glm::vec3& e)
	{
		const auto s = 1e-8;
		return (fabs(e.x) < s) && (fabs(e.y) < s) && (fabs(e.z) < s);
	}

	class Lambertian : public RayMaterial
	{
	public:
		Lambertian(const glm::vec3& color) : albedo(color) {}

		bool Scatter(const Ray& inRay, const HitRecord& hit, glm::vec3& attenuation, Ray& scatteredRay) const override
		{
			glm::vec3 scatterDirection = hit.normal + RandomUnitVector();
			
			if (IsZero(scatterDirection))
				scatterDirection = hit.normal;
			
			scatteredRay = Ray(hit.point, scatterDirection);
			attenuation = albedo;

			return true;
		}

	private:
		glm::vec3 albedo;
	};

	glm::vec3 Reflect(const glm::vec3& vector, const glm::vec3& normal)
	{
		return vector - 2 * glm::dot(vector, normal) * normal;
	}

	class Metal : public RayMaterial
	{
	public:
		Metal(const glm::vec3& color, float f) : albedo(color), fuzz(f < 1 ? f : 1) {}

		bool Scatter(const Ray& inRay, const HitRecord& hit, glm::vec3& attenuation, Ray& scatteredRay) const override
		{
			glm::vec3 reflected = Reflect(glm::normalize(inRay.direction), hit.normal);

			scatteredRay = Ray(hit.point, reflected + (fuzz * RandomUnitSphere()));
			attenuation = albedo;

			return (glm::dot(scatteredRay.direction, hit.normal) > 0.0f);
		}

	private:
		glm::vec3 albedo;
		float fuzz;
	};

	glm::vec3 Refract(const glm::vec3& uv, const glm::vec3& n, float etai_over_etat)
	{
		float cosTheta = fmin(dot(-uv, n), 1.0);
		glm::vec3 rOutPerp = etai_over_etat * (uv + cosTheta * n);
		glm::vec3 rOutParallel = (float)- glm::sqrt(fabs(1.0 - glm::length2(rOutPerp))) * n;
		return rOutPerp + rOutParallel;
	}

	bool Reflectance(float cosine, float refIndex)
	{
		float r0 = (1.0f - refIndex) / (1.0f + refIndex);
		r0 = r0 * r0;
		return r0 + (1.0f - r0) * pow((1.0f - cosine), 5);
	}

	class Dielectric : public RayMaterial
	{
	public:
		Dielectric(float ir) : refractionIndex(ir) {}

		bool Scatter(const Ray& inRay, const HitRecord& hit, glm::vec3& attenuation, Ray& scatteredRay) const override
		{
			attenuation = { 1.0f, 1.0f, 1.0f };
			float refractionRatio = hit.frontFace ? (1.0f / refractionIndex) : refractionIndex;

			glm::vec3 unitDirection = glm::normalize(inRay.direction);

			float cosTheta = fmin(dot(-unitDirection, hit.normal), 1.0);
			float sinTheta = glm::sqrt(1.0 - cosTheta * cosTheta);

			bool cannotRefract = (refractionIndex * sinTheta) > 1.0;

			glm::vec3 direction;

			if (cannotRefract || Reflectance(cosTheta, refractionIndex) > FloatRandom())
				direction = glm::reflect(unitDirection, hit.normal);
			else
				direction = glm::refract(unitDirection, hit.normal, refractionIndex);

			scatteredRay = Ray(hit.point, direction);
			return true;
		}

	private:
		float refractionIndex;
	};
}