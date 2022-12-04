#include "RendererLayer.h"

#include "Hittable.h"
#include "Sphere.h"
#include "Utils.h"
#include "RayMaterial.h"
#include "Camera.h"

#include <ImGui/imgui.h>

namespace Lux
{
	RendererLayer::RendererLayer()
	{
	}
	
	RendererLayer::~RendererLayer()
	{
	}
	
	void RendererLayer::OnCreate()
	{
	}
	
	void RendererLayer::OnDestroy()
	{
	}
	
	void RendererLayer::Update(Timer timer)
	{
		
	}
	
	void RendererLayer::RenderImGui()
	{
		// Dockspace
		static bool dockspaceOpen = true;
		static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
			windowFlags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, windowFlags);
		ImGui::PopStyleVar(3);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID id = ImGui::GetID("Dockspace");
			ImGui::DockSpace(id, { 0.0f, 0.0f }, dockspaceFlags);
		}


		// Renderer
		ImGui::Begin("Settings");

		if (ImGui::Button("Render"))
		{
			Render();
		}

		ImGui::End();

		ImGui::Begin("Viewport");

		viewportWidth = ImGui::GetContentRegionAvail().x;
		viewportHeight = ImGui::GetContentRegionAvail().y;

		camera.SetDimensions(viewportWidth, viewportHeight);

		if (image)
			ImGui::Image((void*)image->GetID(), { (float)image->GetWidth(), (float)image->GetHeight() });

		ImGui::End();


		ImGui::End();
	}
	
	void RendererLayer::OnEvent(Event& e)
	{
	}

	float HittedSphere(const glm::vec3& center, float radius, const Ray& ray)
	{
		// (A + tB + C) * (A + tB + C) = r2

		glm::vec3 originMinusCenter = ray.origin - center;

		// The sqrt length is the same as the dot product of direction, and it's more efficient
		float a = glm::length2(ray.direction);
		float b = /* 2.0f * */ glm::dot(originMinusCenter, ray.direction);
		float c = glm::length2(originMinusCenter) - (radius * radius);
		float discriminant = (b * b) - (a * c);

		if (discriminant < 0)
			return -1.0f;
		else
			return (- b - glm::sqrt(discriminant)) / a;
	}

	glm::vec3 RayColor(const Ray& ray, const Hittable& world, int depth)
	{
		if (depth <= 0)
			return glm::vec3(0.0f);

		HitRecord hit = {};

		if (world.Hit(ray, 0.001f, std::numeric_limits<float>::infinity(), hit))
		{
			Ray scattered = { glm::vec3(0.0f), glm::vec3(0.0f) };
			glm::vec3 attenuation = {};

			if (hit.material->Scatter(ray, hit, attenuation, scattered))
				return attenuation * RayColor(scattered, world, depth - 1);
			
			return glm::vec3(0.0f);
			//return 0.5f * (hit.normal + glm::vec3(1.0f, 1.0f, 1.0f));
			//return 0.5f * (hit.normal + 1.0f);
		}

		glm::vec3 dir = glm::normalize(ray.direction);
		float t = 0.5f * (dir.y + 1.0f);

		return (1.0f - t) * glm::vec3(1.0f, 1.0f, 1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f);
	}

	glm::vec3 ColorRandom()
	{
		return glm::vec3(FloatRandom(), FloatRandom(), FloatRandom());
	}

	glm::vec3 ColorRandom(float min, float max)
	{
		return glm::vec3(FloatRandom(min, max), FloatRandom(min, max), FloatRandom(min, max));
	}

	HittableList RandomScene() 
	{
		HittableList world;

		auto groundMaterial = std::make_shared<Lambertian>(glm::vec3(0.5f, 0.5f, 0.5f));
		world.Add(std::make_shared<Sphere>(glm::vec3(0.0f, -1000.0f, 0.0f), 1000.0f, groundMaterial));

		for (int a = -11; a < 11; a++) 
		{
			for (int b = -11; b < 11; b++) 
			{
				auto chooseMat = FloatRandom();
				glm::vec3 center(a + 0.9f * FloatRandom(), 0.2f, b + 0.9f * FloatRandom());

				if (glm::length((center - glm::vec3(4.0f, 0.2f, 0.0f))) > 0.9f)
				{
					if (chooseMat < 0.8) 
					{
						// Diffuse
						auto albedo = ColorRandom() * ColorRandom();
						world.Add(std::make_shared<Sphere>(center, 0.2, std::make_shared<Lambertian>(albedo)));
					}
					else if (chooseMat < 0.95) 
					{
						// Metal
						auto albedo = ColorRandom(0.5f, 1.0f);
						auto fuzz = FloatRandom(0.0f, 0.5f);
						world.Add(std::make_shared<Sphere>(center, 0.2, std::make_shared<Metal>(albedo, fuzz)));
					}
					else 
					{
						// Glass
						world.Add(std::make_shared<Sphere>(center, 0.2, std::make_shared<Dielectric>(1.5)));
					}
				}
			}
		}

		auto material1 = std::make_shared<Dielectric>(1.5);
		world.Add(std::make_shared<Sphere>(glm::vec3(0, 1, 0), 1.0, material1));

		auto material2 = std::make_shared<Lambertian>(glm::vec3(0.4, 0.2, 0.1));
		world.Add(std::make_shared<Sphere>(glm::vec3(-4, 1, 0), 1.0, material2));

		auto material3 = std::make_shared<Metal>(glm::vec3(0.7, 0.6, 0.5), 0.0);
		world.Add(std::make_shared<Sphere>(glm::vec3(4, 1, 0), 1.0, material3));

		return world;
	}
	
	void RendererLayer::Render()
	{
		float aspectRatio = 16.0f / 9.0f;
		uint32_t imgWidth = 400;
		uint32_t imgHeight = (imgWidth / aspectRatio);
		uint16_t focalLength = 1.0f;

		uint32_t* data = new uint32_t[imgWidth * imgHeight];

		float viewHeight = 2.0f;
		float viewWidth = viewHeight * aspectRatio;

		float numSamples = 32;
		int maxDepth = 8;

		// World
		HittableList world = {}/*RandomScene()*/;

		glm::vec3 lookFrom = { 13.0f, 2.0f, 3.0f };
		glm::vec3 lookAt = { 0.0f, 0.0f, 0.0f };
		glm::vec3 up = { 0.0f, 1.0f, 0.0f };

		float distToFocus = 10.0f;
		float aperture = 0.1f;

		// Creating camera
		Camera cam = Camera(lookFrom, lookAt, up, 20.0f, aspectRatio, aperture, distToFocus);

		float scale = 1.0f / numSamples;

		for (int j = imgHeight - 1; j >= 0; --j)
		{
			for (int i = 0; i < imgWidth; ++i)
			{
				glm::vec3 color = glm::vec3(0.0f);

				/*for (int k = 0; k < numSamples; ++k)
				{*/
					auto u = float(i) / (imgWidth - 1);
					auto v = float(j) / (imgHeight - 1);
					Ray ray = cam.GetRay(u, v);

					color += RayColor(ray, world, maxDepth);
				//}
				color.x = 256 * glm::clamp(glm::sqrt(color.x /** scale*/), 0.0f, 0.999f);
				color.y = 256 * glm::clamp(glm::sqrt(color.y /** scale*/), 0.0f, 0.999f);
				color.z = 256 * glm::clamp(glm::sqrt(color.z /** scale*/), 0.0f, 0.999f);

				uint32_t finalColor = 0xff000000 | ((int)color.b << 16) | ((int)color.g << 8) | (int)color.r;
				data[((imgHeight - 1 - j) * imgWidth) + i] = finalColor;
			}
		}

		if (!image) image = CreateSharedPtr<Texture2D>(data, imgWidth, imgHeight);
	}
}