#include <iostream>
#include "rendering/Texture.h"
#include "rendering/RenderDevice.h"
#include "rendering/RenderTarget.h"
#include "rendering/Sampler.h"
#include <Core/Application/Application.h>
#include "rendering/Shader.h"
#include <Editor/EditorRenderContext.h>
#include <Core/Systems/RenderSystem.h>
#include "common/CommonTypes.h"
#include "rendering/AssetLoader.h"
#include <fstream>
#include <sstream>

void processInput(GLFWwindow *window);

int main()
{
	Application* App = Application::Create(1024, 900);

	App->Run();

    return 0;
}


