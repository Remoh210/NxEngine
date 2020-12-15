#include "EditorUI.h"

#include "imgui.h"
#include "Common/CommonTypes.h"
#include "Core/Application/Settings/GlobalSettings.h"

bool EditorUI::init = true;
ImVec2 EditorUI::SceneViewSize;
ImVec2 EditorUI::SceneViewPos;

void EditorUI::Initialize(NString apiVersion, Window* window)
{
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplGlfw_InitForOpenGL(window->GetWindowHandle(), true);
	NString glVersionImgui;
	glVersionImgui = "#version " + apiVersion;
	ImGui_ImplOpenGL3_Init(glVersionImgui.c_str());
	ImGui::StyleColorsDark();
}

void EditorUI::DrawEditorView(EditorRenderContext* editorContext)
{


	static ImGuiID dockspaceID = 0;
	bool active = true;
	if (ImGui::Begin("Master Window", &active))
	{
		ImGui::TextUnformatted("DockSpace below");
	}
	if (active)
	{
		// Declare Central dockspace
		dockspaceID = ImGui::GetID("HUB_DockSpace");
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	}
	ImGui::End();





	ImVec2 SceneViewSize;
	if (init)
	{
		SceneViewSize = ImVec2(GlobalSettings::GetWindowWidth() / 2, GlobalSettings::GetWindowHeight()/2);
		SceneViewPos = ImVec2(GlobalSettings::GetWindowWidth() / 2, GlobalSettings::GetWindowHeight() / 2);
		ImGui::SetNextWindowSize(SceneViewSize);
		ImGui::SetNextWindowPos(SceneViewPos);
		init = false;
	}
	ImGui::Begin("Main2", nullptr, ImGuiTabBarFlags_None);
	//ImGui::BeginChild("MainViews", VecScreen, true);
	ImGuiTabBarFlags tab_bar_flags2 = ImGuiTabBarFlags_None;
	ImGui::BeginTabBar("MyTabBar2", tab_bar_flags2);
	
	if (ImGui::BeginTabItem("Scene"))
	{
		SceneViewSize = ImGui::GetContentRegionAvail();
		//editorContext->ResizeViewPort(SceneViewSize.x, SceneViewSize.y);
		ImGui::Image((void*)editorContext->GetScreenTexture()->GetId(), SceneViewSize, ImVec2(0, 1), ImVec2(1, 0));
		ImGui::EndTabItem();
	}
	//if (ImGui::BeginTabItem("Albedo"))
	//{
	//	if (GBuffer)
	//	{
	//		ImGui::Image((void*)GBuffer->colourTexture_0_ID, SceneViewSize, ImVec2(0, 1), ImVec2(1, 0));
	//	}
	//	ImGui::EndTabItem();
	//}
	//if (ImGui::BeginTabItem("Position"))
	//{
	//	if (GBuffer)
	//	{
	//		ImGui::Image((void*)GBuffer->vertexWorldPos_2_ID, SceneViewSize, ImVec2(0, 1), ImVec2(1, 0));
	//	}
	//	ImGui::EndTabItem();
	//}
	//if (ImGui::BeginTabItem("Normal"))
	//{
	//	if (GBuffer)
	//	{
	//		ImGui::Image((void*)GBuffer->normalTexture_1_ID, SceneViewSize, ImVec2(0, 1), ImVec2(1, 0));
	//	}
	//	ImGui::EndTabItem();
	//}
	ImGui::EndTabBar();
	ImGui::End();
	//ImGui::EndChild();





	//ImGui::SetNextWindowDockID(dockspaceID, ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Dockable Window"))
	{
		ImGui::TextUnformatted("Test");
	}
	ImGui::End();
}

void EditorUI::Shutdown()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void EditorUI::UpdateWindows()
{
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
}

void EditorUI::Draw()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static const float identityMatrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f };

void EditorUI::EditTransform(const float* cameraView, float* cameraProjection, float* matrix, bool editTransformDecomposition)
{
}
