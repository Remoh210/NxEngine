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
	static void DrawEditorView(EditorRenderContext* editorContext);
	static void DrawInspector();
	static void Shutdown();
	static void UpdateWindows();
	static void Draw();
	
	//static void NewFrame();
	//static void DrawGizmo();
	static void EditorUI::EditTransform(const float* cameraView, float* cameraProjection, 
		float* matrix, bool editTransformDecomposition);
	//static void ShutDown();

private:
	static ImVec2 SceneViewSize;
	static ImVec2 SceneViewPos;
	static bool init;
};