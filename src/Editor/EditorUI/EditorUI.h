#pragma once

#include "Common/dataStructs/String.h"
#include "rendering/Window.h"
#include "Editor/EditorRenderContext.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

class EditorUI
{
public:

	static void Initialize(NString apiVersion, Window* window);
	static void DrawSceneTree();
	static void DrawMenuBar();
	static void ResetDocking();
	static void DrawSceneView(EditorRenderContext* editorContext);
	static void DrawMainDockWidnow();
	static void DrawEditorView(EditorRenderContext* editorContext);
	static void DrawInspector();
	static void Shutdown();
	static void UpdateWindows();
	static void Draw();
	static void OnWindowResized(float x, float y);


private:
	//Renema To Master Window
	static ImVec2 SceneViewSize;
	static ImVec2 SceneViewPos;
	static int32 selectedEntity;
	static bool init;
	static bool bMasterWindowActive;
};