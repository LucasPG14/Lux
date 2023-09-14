# Lux

Lux is a rendering engine created from scratch using OpenGL and C++ for my final degree project at the UPC-CITM. In this engine, we find an implementation of a path tracing.

The aim of this project is to implement a path tracing renderer where you can import models, modify the materials and make a render of that scene you created. Finally, when the render its over, you can save this render into an image in png format.

The basis of this project was made following [TheCherno's](https://www.youtube.com/c/TheChernoProject) [Game Engine Series](https://www.youtube.com/playlist?list=PLlrATfBNZ98dC-V-N3m0Go4deliWHPFwT) playlist. All the stuff related to the core engine is from this playlist, but all the renderer related things are made by myself.

![](https://github.com/LucasPG14/Lux/blob/main/Screenshoots/Render.png)

![](https://github.com/LucasPG14/Lux/blob/main/Screenshoots/RendererCompleto.png)

## Controls

To move the camera:

- W: Moves forward
- A: Moves to the left
- S: Moves backwards
- D: Moves to the right

If you do this using SHIFT, the camera will move faster.

- CTRL + S: Saves the scene
- CTRL + SHIFT + S: Saves the scene as a new one
- CTRL + N: Creates a new scene
- CTRL + O: Opens a file dialog to open a scene
- ALT + F4: Closes the app
- DEL: Will delete the entity its selected

## How to use Lux

There are some scene examples but the user can create new ones. When you open the app, the scene only have a directional light. You can import your models by dropping them into the content browser. Then from there, you can add them to the scene by dropping them into the viewport. You can select your objects and modify their materials properties in order to create a beatiful scene.
When you have finished your scene, you can choose the number of samples do you want the render to have and also the number of rays bounces, which is limited to 8 due to performance reasons. Once the render is finished, you can save it as a png image.

IMPORTANT: To import models, they need to be in a single mesh, otherwise it will only import the first mesh. Also, the path tracing it's a very expensive technique so you can't import heavy models. In the resource manager there are few basic objects like a sphere, cube and a cone.

## Dependencies

Lux uses several open source libraries. Here I will leave a link to all of them:

- [Assimp](https://github.com/assimp/assimp)
- [Glad](https://glad.dav1d.de/)
- [GLFW](https://github.com/glfw/glfw)
- [GLM](https://github.com/g-truc/glm)
- [ImGui](https://github.com/ocornut/imgui)
- [ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo)
- [Optick](https://github.com/bombomby/optick)
- [SPDLOG](https://github.com/gabime/spdlog)
- [STB](https://github.com/nothings/stb)
- [YAML](https://github.com/jbeder/yaml-cpp)
