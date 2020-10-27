#pragma once
#include <rendering/Window.h>
#include <Common/dataStructs/String.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <Core/Camera/Camera.h>
#include <Core/ECS/ECS.h>



class Application
{
public:
	static Application* Create(float Width, float Height);

	int Run();

	~Application();

	bool IsRunning();
	inline GLFWwindow* GetWindow()
	{
		return window;
	}

	static Camera* GetMainCamera();

	void ResizeWindow(uint32 width, uint32 height);
	void processInput(GLFWwindow *window);


	static bool loadTextFileWithIncludes(NString& output, const NString& fileName,
		const NString& includeKeyword);

private:
	Application(float Width, float Height);

	void Initialize();

	void DisplayEntyties(ECS& ecs);

	void ShowContentWindow(ECS& ecs);
	void ShowSceneObjectList(ECS& ecs);
	void ShowInspector(ECS & ecs);
	void ShowEditor(GLint EditorTex);

	void loadEntityToScene(ECS& ecs, string name);

	void GUI_ShowMenuBar(ECS& ecs);

	Map<string, string> MapModelPaths;

	bool isAppRunning;
	static uint32 numInstances;
	static Camera* MainCamera;
	GLFWwindow* window;


	uint32 windowWidth;
	uint32 windowHeight;

	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	static double gScrollOffset;
	static double gXoffset;
	static double gYoffset;

	double deltaTime;

};
