#include <Amethyst.h>

#include "imgui/imgui.h"

class ExampleLayer : public Amethyst::Layer
{
public:
	ExampleLayer() : Layer("Example")
	{
		vao.reset(Amethyst::VertexArray::Create());

		float vertices[21] =
		{
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f
		};

		std::shared_ptr<Amethyst::VertexBuffer> vbo;
		vbo.reset(Amethyst::VertexBuffer::Create(vertices, sizeof(vertices)));

		{
			Amethyst::BufferLayout layout =
			{
				{Amethyst::ShaderDataType::FLOAT3, "position"},
				{Amethyst::ShaderDataType::FLOAT4, "color"},
				//{ShaderDataType::FLOAT3, "normal"}
			};

			vbo->SetLayout(layout);
		}

		vao->AddVertexBuffer(vbo);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<Amethyst::IndexBuffer> ebo;
		ebo.reset(Amethyst::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		vao->AddIndexBuffer(ebo);

		squareVA.reset(Amethyst::VertexArray::Create());

		float vertices2[20] =
		{
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		std::shared_ptr<Amethyst::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Amethyst::VertexBuffer::Create(vertices2, sizeof(vertices2)));

		Amethyst::BufferLayout layout2 =
		{
			{Amethyst::ShaderDataType::FLOAT3, "position"},
			{Amethyst::ShaderDataType::FLOAT2, "texCoord"},
			//{ShaderDataType::FLOAT3, "normal"}
		};

		vertexBuffer->SetLayout(layout2);
		squareVA->AddVertexBuffer(vertexBuffer);

		uint32_t indices2[6] = { 0, 1, 2, 2, 3, 0 };
		std::shared_ptr<Amethyst::IndexBuffer> indexBuffer;
		indexBuffer.reset(Amethyst::IndexBuffer::Create(indices2, sizeof(indices2) / sizeof(uint32_t)));
		squareVA->AddIndexBuffer(indexBuffer);

		std::string vertex = R"(
			#version 330 core

			layout(location = 0) in vec3 aPosition;
			layout(location = 1) in vec4 aColor;

			uniform mat4 view;
			uniform mat4 projection;
			uniform mat4 model;

			out vec4 vColor;
			
			void main()
			{
				vColor = aColor;
				gl_Position = projection * view * model * vec4(aPosition, 1.0);
			}
		
		)";

		std::string fragment = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec4 vColor;

			void main()
			{
				color = vColor;
			}
		
		)";

		shader.reset(Amethyst::Shader::Create(vertex, fragment));

		std::string vertex2 = R"(
			#version 330 core

			layout(location = 0) in vec3 aPosition;

			uniform mat4 view;
			uniform mat4 projection;
			uniform mat4 model;			

			out vec3 vPosition;
			
			void main()
			{
				vPosition = aPosition;
				gl_Position = projection * view * model * vec4(aPosition, 1.0);
			}
		
		)";

		std::string fragment2 = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec3 vPosition;

			void main()
			{
				color = vec4(0.2, 0.3, 0.8, 1.0);
			}
		
		)";

		shader2.reset(Amethyst::Shader::Create(vertex2, fragment2));
	
		std::string vertexTexture = R"(
			#version 330 core

			layout(location = 0) in vec3 aPosition;
			layout(location = 1) in vec2 aTexCoord;

			uniform mat4 view;
			uniform mat4 projection;
			uniform mat4 model;			

			out vec2 texCoord;
			
			void main()
			{
				texCoord = aTexCoord;
				gl_Position = projection * view * model * vec4(aPosition, 1.0);
			}
		
		)";

		std::string textureFragment = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			in vec2 texCoord;

			uniform sampler2D ourTexture;

			void main()
			{
				color = texture(ourTexture, texCoord);
			}
		
		)";

		texture.reset(Amethyst::Shader::Create(vertexTexture, textureFragment));
	
		tex.reset(Amethyst::Texture2D::Create("assets/textures/bakeHouse.png"));
		logo.reset(Amethyst::Texture2D::Create("assets/textures/deadpool.png"));
	
		texture->Bind();
		texture->UploadUniformInt("ourTexture", 0);
	}

	~ExampleLayer() {}

	void Update(Amethyst::Timer timer) override
	{
		camera.Update(timer);
		AMT_TRACE("Delta time: {0}s ({1})ms", timer.GetSeconds(), timer.GetMilliSeconds());
		//AMT_INFO("ExampleLayer::Update");
		glm::mat4 model(1.0f);

		Amethyst::RenderOrder::ClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Amethyst::RenderOrder::Clear();

		Amethyst::Renderer::BeginScene();

		tex->Bind();
		texture->Bind();
		texture->UploadUniformMat4("view", camera.GetViewMatrix());
		texture->UploadUniformMat4("projection", camera.GetProjectionMatrix());
		texture->UploadUniformMat4("model", model);
		Amethyst::Renderer::Submit(squareVA);
		logo->Bind();
		Amethyst::Renderer::Submit(squareVA);
		//shader->Bind();
		//shader->UploadUniformMat4("view", camera.GetViewMatrix());
		//shader->UploadUniformMat4("projection", camera.GetProjectionMatrix());
		//shader->UploadUniformMat4("model", model);
		//Amethyst::Renderer::Submit(vao);

		Amethyst::Renderer::EndScene();
	}

	void RenderImGui() override
	{
		ImGui::Begin("Hello");
		ImGui::End();
	}

	void OnEvent(Amethyst::Event& event) override
	{
		AMT_TRACE("{0}", event);
	}
private:
	std::shared_ptr<Amethyst::Shader> shader;
	std::shared_ptr<Amethyst::VertexArray> vao;

	std::shared_ptr<Amethyst::Shader> shader2;
	std::shared_ptr<Amethyst::Shader> texture;
	std::shared_ptr<Amethyst::Texture2D> tex;
	std::shared_ptr<Amethyst::Texture2D> logo;
	std::shared_ptr<Amethyst::VertexArray> squareVA;

	Amethyst::PerspectiveCamera camera;
};

class AmethystEditor : public Amethyst::Application
{
public:
	AmethystEditor()
	{
		PushLayer(new ExampleLayer);
	}

	~AmethystEditor()
	{

	}

};

Amethyst::Application* Amethyst::CreateApp()
{
	return new AmethystEditor();
}