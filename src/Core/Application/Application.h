#pragma once
#include <Common/dataStructs/Map.h>
#include <rendering/Window.h>
#include <Common/dataStructs/String.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <Core/Camera/Camera.h>
#include "rendering/RenderDevice.h"
#include "rendering/RenderTarget.h"
#include "rendering/Window.h"
#include "Editor/EditorRenderContext.h"
#include "rendering/Sampler.h"
#include <Core/ECS/ECS.h>



class Application
{
public:
	static Application* Create(float Width, float Height);

	void DrawDebugWindow();
	int Run();

	~Application();

	bool IsRunning();
	inline Window* GetWindow()
	{
		return window;
	}

	static Camera* GetMainCamera();
	static inline bool GetIsPIE() { return bPlayingInEditor; };

	void ResizeWindow(uint32 width, uint32 height);
	void processInput(GLFWwindow *window);


	static bool loadTextFileWithIncludes(NString& output, const NString& fileName,
		const NString& includeKeyword);

private:
	Application(float Width, float Height);

	void Initialize();

	void ShutDown();

	void LoadDefaultScene();

	void DisplayEntyties();

	void ShowContentWindow();
	void ShowSceneObjectList();
	void ShowInspector();
	void ShowEditor();

	void GUI_ShowMenuBar();

	static void TogglePIE();

	NxMap<string, string> MapModelPaths;

	bool isAppRunning;
	static uint32 numInstances;
	static Camera* MainCamera;
	static Window* window;
	static bool bPlayingInEditor;
	


	uint32 windowWidth;
	uint32 windowHeight;

	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static double gScrollOffset;
	static double gXoffset;
	static double gYoffset;

	double deltaTime;

private:
	ECS::World* world;
	Sampler* editorSampler;
	RenderTarget* editorRenderTarget;
	EditorRenderContext* editorRenderContext;
	RenderDevice* renderDevice;
	

	ECS::EntitySystem* physicsSystem;
};
