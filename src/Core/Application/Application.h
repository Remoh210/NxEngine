//#pragma once
//#include "Common/Common.h"
//#include <imgui_impl_glfw.h>
//#include <imgui_impl_opengl3.h>
//#include "../Rendering/LOpenGL/camera.h"
//#include "../ECS/ECS.h"
//#include <imgui/imgui.h>
//
//
//
//class Application
//{
//public:
//	static Application* Create(float Width, float Height);
//
//	int Run();
//
//	~Application();
//
//	bool IsRunning();
//	inline GLFWwindow* GetWindow()
//	{
//		return window;
//	}
//
//	static Camera* GetMainCamera();
//
//	void ResizeWindow(uint32 width, uint32 height);
//	void processInput(GLFWwindow *window);
//
//private:
//	Application(float Width, float Height);
//	void GlInit();
//
//	
//	void DisplayEntyties(ECS& ecs);
//
//	void ShowContentWindow(ECS& ecs);
//	void ShowSceneObjectList(ECS& ecs);
//	void ShowInspector(ECS & ecs);
//	void ShowEditor(GLint EditorTex);
//
//	void loadEntityToScene(ECS& ecs, string name);
//
//	void GUI_ShowMenuBar(ECS& ecs);
//
//	Map<string, string> MapModelPaths;
//
//	bool isAppRunning;
//	static uint32 numInstances;
//	static Camera* MainCamera;
//	GLFWwindow* window;
//
//	//Array<>
//
//	uint32 windowWidth;
//	uint32 windowHeight;
//
//	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//
//	static double gScrollOffset;
//	static double gXoffset;
//	static double gYoffset;
//
//	double deltaTime;
//
//};