#include "EditorUI.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Common/CommonTypes.h"
#include "Common/dataStructs/String.h"
#include "Common/Common.h"
#include "Core/FileSystem/FileSystem.h"
#include "Core/Application/SceneManager/SceneManager.h"
#include "Core/Application/Settings/GlobalSettings.h"
#include "Core/Components/TransformComponent/TransformComponent.h"
#include "Core/Components/SkinnedMeshComponent/SkinnedMeshComponent.h"



bool EditorUI::init = true;
bool EditorUI::bMasterWindowActive = true;


vec2 EditorUI::SceneViewSize;
vec2 EditorUI::SceneViewPos;
int32 EditorUI::selectedEntity = -1;


/////////////////////////////////////////////////////////////////////////////////////////

void reflectUI(rttr::instance obj);
bool reflectBasicType(const rttr::type& t, rttr::variant& var, NString propName, rttr::variant min, rttr::variant max);
void reflectVariant(rttr::variant& var, NString propName, rttr::variant& min, rttr::variant& max);
void reflectArray(rttr::variant_sequential_view view, NString propName, rttr::variant& min, rttr::variant& max);
void reflectMap(rttr::variant_associative_view view, NString propName, rttr::variant& min, rttr::variant& max);
void reflectVec(rttr::variant& var, NString propName, uint32 size);
void reflectTexture(rttr::variant& var, NString propName, uint32 size = 128);


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

	ImGuiStyle* style = &ImGui::GetStyle();

	style->FrameRounding = 0.0f;
	style->WindowRounding = 0.0;
}

void EditorUI::DrawSceneTree()
{
	ImGui::Begin("SceneTree", nullptr, ImGuiTabBarFlags_None);

	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_OpenOnDoubleClick;


		static int selection_mask = (1 << 2);
		int node_clicked = -1;
		int i = 0;
		for (int entCount = 0; entCount < SceneManager::currentScene.GetNumObjects(); entCount++)
		{
			// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
			ImGuiTreeNodeFlags node_flags = base_flags;
			const bool is_selected = (selection_mask & (1 << i)) != 0;
			if (is_selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;

			NString name = SceneManager::currentScene.sceneObjects[entCount]->objectName;
			bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, name.c_str(), i);
			if (ImGui::IsItemClicked()) 
			{ 
				selectedEntity = i;
				node_clicked = i;
			}
			if (node_open)
			{
				ImGui::TreePop();
			}
			i++;
		}
		if (node_clicked != -1)
		{
			if (ImGui::GetIO().KeyCtrl)
				selection_mask ^= (1 << node_clicked);
			else
				selection_mask = (1 << node_clicked);
		}

	ImGui::End();
}

void EditorUI::DrawMenuBar()
{
	ImGui::BeginMenuBar();
	if (ImGui::BeginMenu("Menu"))
	{
		if (ImGui::MenuItem("New")) 
		{
			//Create and load new scene
		}
		if (ImGui::MenuItem("Open", "Ctrl+O"))
		{
				
		}
		if (ImGui::MenuItem("Save Scene", "Ctrl+S")) 
		{  
			SceneManager::SaveScene("TestSceneRTT");
		}
		if (ImGui::MenuItem("Load Scene", "Ctrl+L"))
		{
		
			//SceneManager::LoadScene("TestScene.json");
		}
		if (ImGui::MenuItem("Save As.."))
		{
			ImGui::Separator();
			//if (ImGui::BeginMenu("Options"))
			//{
			//	static bool enabled = true;
			//	ImGui::MenuItem("Enabled", "", &enabled);
			//	ImGui::BeginChild("child", ImVec2(0, 60), true);
			//	for (int i = 0; i < 10; i++)
			//		ImGui::Text("Scrolling Text %d", i);
			//	ImGui::EndChild();
			//	static float f = 0.5f;
			//	static int n = 0;
			//	static bool b = true;
			//	ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
			//	ImGui::InputFloat("Input", &f, 0.1f);
			//	ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
			//	ImGui::Checkbox("Check", &b);
			//	ImGui::EndMenu();
			//}
		}

		if (ImGui::MenuItem("Reset layout", "Ctrl+R")) 
		{  
			//ResetDocking();
		}
		ImGui::EndMenu();
	}
	ImGui::EndMenuBar();
}

void EditorUI::ResetDocking()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGuiID dockspace_id =  ImGui::GetID("MasterWindowDockId");
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
	
	ImGui::DockBuilderRemoveNode(dockspace_id); // clear any previous layout
	ImGui::DockBuilderAddNode(dockspace_id, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
	ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);
	
	//ImGuiID dock_id_top = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Up, 0.2f, nullptr, &dockspace_id);
	ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Down, 0.25f, nullptr, &dockspace_id);
	ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.2f, nullptr, &dockspace_id);
	ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.15f, nullptr, &dockspace_id);
	
	//Left
	ImGui::DockBuilderDockWindow("SceneTree", dock_id_left);
	ImGui::DockBuilderDockWindow("TestWindow", dock_id_left);
	
	//Centre
	ImGui::DockBuilderDockWindow("SceneView", dockspace_id);

	//Right
	ImGui::DockBuilderDockWindow("Inspector", dock_id_right);

	//Down
	ImGui::DockBuilderDockWindow("DebugWindow", dock_id_down);
	ImGui::DockBuilderDockWindow("Log", dock_id_down);
	
	ImGui::DockBuilderFinish(dockspace_id);
	

	//TODO: Would be nice to set active tabs properly instead of changing their order in the main draw func
	// if(ImGui::DockBuilderGetNode(dock_id_left) &&ImGui::DockBuilderGetNode(dock_id_left)->TabBar)
	// {
	// 		ImGui::DockBuilderGetNode(dock_id_left);
	// 		ImGui::DockBuilderGetNode(dock_id_left)->TabBar->NextSelectedTabId = ImGui::GetID("TestWindow");
	// }
	//
}

void EditorUI::DrawSceneView(EditorRenderContext* editorContext)
{
	ImGuiWindowFlags scene_window_flags = 0;
	//scene_window_flags |= ImGuiWindowFlags_NoTitleBar;
	//scene_window_flags |= ImGuiWindowFlags_NoDecoration;
	//scene_window_flags |= ImGuiWindowFlags_NoScrollbar;
	//scene_window_flags |= ImGuiWindowFlags_NoDocking;
	
	ImGui::Begin("SceneView", NULL, scene_window_flags);
	//ImGuiTabBarFlags tab_bar_flags2 = ImGuiTabBarFlags_None;
	//ImGui::BeginTabBar("MyTabBar2", tab_bar_flags2);
	//if (ImGui::BeginTabItem("Scene"))
	{
		const ImVec2 WindowSize = ImGui::GetContentRegionAvail();
		if (SceneViewSize.x != WindowSize.x || SceneViewSize.y != WindowSize.y)
		{
			SceneViewSize.x = WindowSize.x;
			SceneViewSize.y = WindowSize.y;
			
			//DEBUG_LOG_TEMP("RESIZE()");
			editorContext->ResizeRenderTargets(WindowSize.x, WindowSize.y);
	
		}
		//ImGui::Text("Size: %f : %f", SceneViewSize.x, SceneViewSize.y);
		//editorContext->ResizeViewPort(SceneViewSize.x, SceneViewSize.y);
		ImGui::Image((void*)editorContext->GetScreenTexture()->GetId(), WindowSize, ImVec2(0, 1), ImVec2(1, 0));
	//	ImGui::EndTabItem();
	}
	//ImGui::EndTabBar();
	ImGui::End();
}

// Demonstrate creating a simple log window with basic filtering.
void EditorUI::DrawLogWindow()
{
	appLog.Draw("Log");
}

void EditorUI::DrawMainDockWidnow()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoDecoration;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	const ImGuiID dockspaceID = ImGui::GetID("MasterWindowDockId");
	SceneViewPos = vec2(0, 0);
	//ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowPos(viewport->Pos);
	if (ImGui::Begin("MasterWindow", &bMasterWindowActive, window_flags))
	{
		ImGuiDockNodeFlags master_dock_flags = 0;
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), master_dock_flags);
		
		DrawMenuBar();
	}
	ImGui::End();
}

void EditorUI::DrawEditorView(EditorRenderContext* editorContext)
{
	if (init)
	{
		ResetDocking();
	}
	
	DrawMainDockWidnow();
	DrawInspector();
	DrawSceneView(editorContext);
	DrawLogWindow();
	if (ImGui::Begin("TestWindow"))
	{
		ImGui::Text("TEST TEST");
	}
	ImGui::End();
	DrawSceneTree();
	
	
	if (init)
	{
		init = false;
	}
}



void EditorUI::DrawInspector()
{
	if (init)
	{
		ImVec2 size = ImVec2(GlobalSettings::GetWindowWidth() / 4, GlobalSettings::GetWindowHeight() / 2);
		//ImGui::SetNextWindowSize(size);
	}

	ImGui::Begin("Inspector");

	if (selectedEntity != -1)
	{
		ECS::Entity* entity = SceneManager::currentScene.sceneObjects[selectedEntity]->entity;

		ImGui::Text("TransformComponent");
		ECS::ComponentHandle<TransformComponent> transformComp = entity->get<TransformComponent>();
		if (transformComp) { reflectUI(transformComp.get()); }

		ECS::ComponentHandle<StaticMeshComponent> meshComp = entity->get<StaticMeshComponent>();
		if (meshComp) 
		{ 
			ImGui::Text("StaticMeshComponent");
			reflectUI(meshComp.get()); 
		}

		ECS::ComponentHandle<SkinnedMeshComponent> skinnedMeshComp = entity->get<SkinnedMeshComponent>();
		if (skinnedMeshComp)
		{
			ImGui::Text("Skeletal Mesh Component");
			reflectUI(skinnedMeshComp.get());
		}

		ECS::ComponentHandle<LightComponent> lightComp = entity->get<LightComponent>();
		if (lightComp) 
		{ 
			ImGui::Text("LightComponent");
			reflectUI(lightComp.get()); 
		}
		
	}
	//static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow
	//	| ImGuiTreeNodeFlags_OpenOnDoubleClick;

	//static int selection_mask = (1 << 2);
	//int node_clicked = -1;
	//int i = 0;
	//for (auto entity : SceneManager::currentScene.sceneObjects)
	//{
	//	// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
	//	ImGuiTreeNodeFlags node_flags = base_flags;
	//	const bool is_selected = (selection_mask & (1 << i)) != 0;
	//	if (is_selected)
	//		node_flags |= ImGuiTreeNodeFlags_Selected;

	//	bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, entity->get<StaticMeshComponent>()->meshAssetFile.c_str(), i);
	//	if (ImGui::IsItemClicked()) { node_clicked = i; }
	//	if (node_open)
	//	{
	//		ImGui::Text("TransformComponent");
	//		ECS::ComponentHandle<TransformComponent> transformComp = entity->get<TransformComponent>();
	//		reflectUI(transformComp.get());
	//		ImGui::Text("StaticMeshComponent");
	//		ECS::ComponentHandle<StaticMeshComponent> meshComp = entity->get<StaticMeshComponent>();
	//		reflectUI(meshComp.get());
	//		ImGui::TreePop();
	//	}
	//	i++;
	//}
	//if (node_clicked != -1)
	//{
	//	// Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
	//	if (ImGui::GetIO().KeyCtrl)
	//		selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
	//	else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
	//		selection_mask = (1 << node_clicked);           // Click to single-select
	//}

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

void EditorUI::OnWindowResized(float x, float y)
{
	SceneViewSize = vec2(x, y);
}

static const float identityMatrix[16] =
{ 1.f, 0.f, 0.f, 0.f,
	0.f, 1.f, 0.f, 0.f,
	0.f, 0.f, 1.f, 0.f,
	0.f, 0.f, 0.f, 1.f };

void reflectVec(rttr::variant& var, NString propName, uint32 size)
{
	auto value_type = var.get_type();
	auto wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
	bool is_wrapper = wrapped_type != value_type;

	auto child_props = is_wrapper ? wrapped_type.get_properties() : value_type.get_properties();
	if (!child_props.empty())
	{
		rttr::instance obj2 = var;
		rttr::instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;
		const auto prop_list = obj.get_derived_type().get_properties();

		for (auto prop : prop_list)
		{
			NxArray<float> dataArray;
			auto value = prop.get_value(obj);
			auto view = value.create_sequential_view();
			for (auto item : view)
			{
				rttr::variant wrapped_var = item.extract_wrapped_value();
				dataArray.push_back(wrapped_var.to_float());
			}

			if (ImGui::DragFloat3(propName.c_str(), &dataArray[0], 0.05, -FLT_MAX, FLT_MAX, "%.3f"))
			{
				for (int i = 0; i < size; i++)
				{
					view.set_value(i, dataArray[i]);
				}
			}
			prop.set_value(obj, value);
		}
	}
}

void reflectTexture(rttr::variant& var, NString propName, uint32 size)
{
	auto value_type = var.get_type();
	auto wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
	bool is_wrapper = wrapped_type != value_type;

	auto child_props = is_wrapper ? wrapped_type.get_properties() : value_type.get_properties();
	if (!child_props.empty())
	{
		rttr::instance obj2 = var;
		rttr::instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;
		const auto prop_list = obj.get_derived_type().get_properties();

		for (auto prop : prop_list)
		{

			uint32 value = prop.get_value(obj).to_int32();

			//rttr::variant wrapped_var = item.extract_wrapped_value();

			ImGui::Image((void*)value, ImVec2(100, 100));
		}
	}
}

void reflectArray(rttr::variant_sequential_view view, NString propName, rttr::variant& min, rttr::variant& max)
{
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_OpenOnDoubleClick;

	bool base_open = ImGui::TreeNodeEx(propName.c_str(), base_flags);
	if (base_open)
	{
		static int selection_mask = (1 << 2);
		int node_clicked = -1;
		int i = 0;
		for (const auto& item : view)
		{
			// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
			ImGuiTreeNodeFlags node_flags = base_flags;
			const bool is_selected = (selection_mask & (1 << i)) != 0;
			if (is_selected)
				node_flags |= ImGuiTreeNodeFlags_Selected;

			bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, std::to_string(i).c_str(), i);
			if (ImGui::IsItemClicked()) { node_clicked = i; }
			if (node_open)
			{

				if (item.is_sequential_container())
				{
					auto view = item.create_sequential_view();
					reflectArray(view, propName, min, max);
				}
				else
				{
					rttr::variant wrapped_var = item.extract_wrapped_value();
					rttr::type value_type = wrapped_var.get_type();
					if (value_type.is_arithmetic() || value_type == rttr::type::get<std::string>() || value_type.is_enumeration())
					{

						reflectBasicType(value_type, wrapped_var, propName + "##" + std::to_string(i), min, max);
						view.set_value(i, wrapped_var);
					}
					else // object
					{
						reflectUI(wrapped_var);
					}
				}
				ImGui::TreePop();
			}
			i++;
		}
		if (node_clicked != -1)
		{
			if (ImGui::GetIO().KeyCtrl)
				selection_mask ^= (1 << node_clicked);
			else
				selection_mask = (1 << node_clicked);
		}
		//prop.set_value(obj, view);
		ImGui::TreePop();
	}
}

void reflectMap(rttr::variant_associative_view view, NString propName, rttr::variant& min, rttr::variant& max)
{
	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_OpenOnDoubleClick;

	bool base_open = ImGui::TreeNodeEx(propName.c_str(), base_flags);
	if (base_open)
	{
		int i = 0;
		for (const auto& item : view)
		{

			rttr::variant var1 = item.first.extract_wrapped_value();
			rttr::variant var2 = item.second.extract_wrapped_value();
			rttr::type value_type1 = var1.get_type();
			rttr::type value_type2 = var2.get_type();

			if (value_type1.is_arithmetic() || value_type1 == rttr::type::get<std::string>() || value_type1.is_enumeration())
			{
				reflectBasicType(value_type1, var1, propName + "##" + std::to_string(i) + "first", min, max);

			}
			else // object
			{
				reflectUI(var1);
			}

			if (value_type2.is_arithmetic() || value_type2 == rttr::type::get<std::string>() || value_type2.is_enumeration())
			{
				reflectBasicType(value_type1, var2, propName + "##" + std::to_string(i) + "second", min, max);

			}
			else // object
			{
				reflectVariant(var2, propName, min, max);
			}

			view.insert(var1, var2);
		}
		ImGui::TreePop();
	}
}


//void reflectProperty(rttr::property& prop, rttr::instance& obj)
//{
//	auto value_type = prop.get_type();
//	auto wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
//	bool is_wrapper = wrapped_type != value_type;
//
//	NString vType = value_type.get_name().to_string();
//
//	//TODO: Fix wrapped types
//	if (value_type.is_arithmetic() || value_type == rttr::type::get<std::string>() || value_type.is_enumeration())
//	{
//		rttr::variant var = prop.get_value(obj);
//		var = is_wrapper ? var.extract_wrapped_value() : var;
//		rttr::variant max = prop.get_metadata("Max");
//		rttr::variant min = prop.get_metadata("Min");
//		reflectBasicType(is_wrapper ? wrapped_type : value_type, var,
//			prop.get_name().to_string(), max, min);
//		prop.set_value(obj, var);
//	}
//	else if (value_type == rttr::type::get<vec3f>())
//	{
//		rttr::variant var = prop.get_value(obj);
//		reflectVec(var, obj, prop.get_name().to_string(), 3);
//		prop.set_value(obj, var);
//	}
//	else if (value_type == rttr::type::get<Texture>())
//	{
//		rttr::variant var = prop.get_value(obj);
//		reflectTexture(var, obj, prop.get_name().to_string(), 3);
//		//prop.set_value(obj, var);
//	}
//	else if (value_type.is_sequential_container())
//	{
//		rttr::variant var = prop.get_value(obj);
//        auto view = var.create_sequential_view();
//		reflectArray(prop, obj);
//		prop.set_value(obj, var);
//	}
//	else if (value_type.is_associative_container())
//	{
//		rttr::variant var = prop.get_value(obj);
//		auto view = var.create_sequential_view();
//		reflectMap(prop, obj);
//		prop.set_value(obj, var);
//	}
//	else
//	{
//		auto child_props = is_wrapper ? wrapped_type.get_properties() : value_type.get_properties();
//		if (!child_props.empty())
//		{
//			rttr::variant var = prop.get_value(obj);
//			reflectUI(var);
//			prop.set_value(obj, var);
//		}
//
//	}
//}

void reflectVariant(rttr::variant& var, NString propName, rttr::variant& min, rttr::variant& max)
{
	auto value_type = var.get_type();
	auto wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
	bool is_wrapper = wrapped_type != value_type;


	NString vType = value_type.get_name().to_string();

	//TODO: Fix wrapped types
	if (value_type.is_arithmetic() || value_type == rttr::type::get<std::string>() || value_type.is_enumeration())
	{
		var = is_wrapper ? var.extract_wrapped_value() : var;
		reflectBasicType(is_wrapper ? wrapped_type : value_type, var,
			propName, min, max);
	}
	else if (value_type == rttr::type::get<vec3f>())
	{
		reflectVec(var, propName, 3);
	}
	else if (value_type == rttr::type::get<vec4f>())
	{
		reflectVec(var, propName, 4);
	}
	else if (value_type == rttr::type::get<Texture*>())
	{
		reflectTexture(var, propName);
	}
	else if (value_type.is_sequential_container())
	{
		reflectArray(var.create_sequential_view(), propName, min, max);

	}
	else if (value_type.is_associative_container())
	{
		reflectMap(var.create_associative_view(), propName, min, max);
	}
	else
	{
		auto child_props = is_wrapper ? wrapped_type.get_properties() : value_type.get_properties();
		if (!child_props.empty())
		{
			reflectUI(var);
		}

	}
}


bool reflectBasicType(const rttr::type& t, rttr::variant& var, NString propName, rttr::variant min, rttr::variant max)
{
	if (t.is_arithmetic())
	{
		if (t == rttr::type::get<bool>())
		{
			bool value = var.to_bool();
			ImGui::Checkbox(propName.c_str(), &value);
			var = value;
		}
		else if (t == rttr::type::get<char>())
		{

		}
		else if (t == rttr::type::get<int8_t>())
		{

		}
		else if (t == rttr::type::get<int16_t>())
		{

		}
		else if (t == rttr::type::get<int32_t>())
		{

		}
		else if (t == rttr::type::get<int64_t>())
		{

		}
		else if (t == rttr::type::get<uint8_t>())
		{

		}
		else if (t == rttr::type::get<uint16_t>())
		{

		}
		else if (t == rttr::type::get<uint32_t>())
		{

		}
		else if (t == rttr::type::get<uint64_t>())
		{

		}
		else if (t == rttr::type::get<float>())
		{
			float maxValue = 100000.0f;
			float minValue = -100000.0f;
			if (max.is_valid())
				maxValue = max.to_float();
			if (min.is_valid())
				minValue = min.to_float();

			float value = var.to_float();
			float speed = maxValue * 0.005f;
			if (ImGui::DragFloat(propName.c_str(), &value, speed, minValue, maxValue))
				var = value;
		}
		else if (t == rttr::type::get<double>())
		{

		}

		return true;
	}
	else if (t.is_enumeration())
	{
		auto names = t.get_enumeration().get_names();
		NxArray<NString> enumNames;
		for (auto& name : names) { enumNames.push_back(name.to_string()); }

		int item_current = 0;
		NxArray<const char*> enumNames_cStr;
		for (int i = 0; i < enumNames.size(); i++)
		{
			if (var.to_string() == enumNames[i])
			{
				item_current = i;
			}
			enumNames_cStr.push_back(enumNames[i].c_str());
		}

		ImGui::Combo(propName.c_str(), &item_current, enumNames_cStr.data(), enumNames.size());
		var = t.get_enumeration().name_to_value(enumNames[item_current]);
		return true;
	}
	else if (t == rttr::type::get<std::string>())
	{
		ImGui::Text(var.to_string().c_str());
		return true;
	}

	return false;
}


void reflectUI(rttr::instance obj2)
{
	rttr::instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;
	const auto prop_list = obj.get_derived_type().get_properties();

	for (auto prop : prop_list)
	{
		rttr::variant max = prop.get_metadata("Max");
		rttr::variant min = prop.get_metadata("Min");
		NString propName = prop.get_name().to_string();
		auto var = prop.get_value(obj);

		reflectVariant(var, propName, min, max);

		prop.set_value(obj, var);
	}
}