#pragma once

#include "Common/dataStructs/String.h"
#include "rendering/Window.h"
#include "Editor/EditorRenderContext.h"
#include "Core/Engine/Console/NxConsole.h"

class EditorUI
{
public:

	static void Initialize(NString apiVersion, Window* window);
	static void DrawSceneTree();
	static void DrawMenuBar();
	static void ResetDocking();
	static void DrawSceneView(EditorRenderContext* editorContext);
	static void DrawLogWindow();
	static void DrawMainDockWidnow();
	static void DrawEditorView(EditorRenderContext* editorContext);
	static void DrawInspector();
	static void Shutdown();
	static void UpdateWindows();
	static void Draw();
	static void OnWindowResized(float x, float y);


private:
	//Renema To Master Window
	static vec2 SceneViewSize;
	static vec2 SceneViewPos;
	static int32 selectedEntity;
	static bool init;
	static bool bMasterWindowActive;
};