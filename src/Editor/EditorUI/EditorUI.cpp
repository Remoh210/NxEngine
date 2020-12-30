#include "EditorUI.h"

#include "imgui.h"
#include "Common/CommonTypes.h"
#include "Core/Application/SceneManager/SceneManager.h"
#include "Core/Application/Settings/GlobalSettings.h"
#include "Core/Components/TransformComponent.h"

bool EditorUI::init = true;
ImVec2 EditorUI::SceneViewSize;
ImVec2 EditorUI::SceneViewPos;


float floatArray[3] = { 1,
		2,
		3
};


/////////////////////////////////////////////////////////////////////////////////////////

void reflectUI(rttr::instance obj);
bool reflectBasicType(const rttr::type& t, rttr::variant& var, NString propName);
void reflectProperty(rttr::property& prop, rttr::instance& obj);
void reflectArray(rttr::variant_sequential_view& view, NString propName);
void reflectVec(rttr::variant& var, rttr::instance& obj, NString propName, uint32 size);

void reflectVec(rttr::variant& var, rttr::instance& obj, NString propName, uint32 size)
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
			Array<float> dataArray;
			auto value = prop.get_value(obj);
			auto view = value.create_sequential_view();
			for (auto item : view)
			{
				rttr::variant wrapped_var = item.extract_wrapped_value();
				dataArray.push_back(wrapped_var.to_float());
			}

			if(ImGui::DragFloat3(propName.c_str(), &dataArray[0], 0.05, -FLT_MAX, FLT_MAX, "%.3f"))
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

void reflectArray(rttr::variant_sequential_view& view, NString propName)
{
	int i = 0;
	for (const auto& item : view)
	{
		if (item.is_sequential_container())
		{
			reflectArray(item.create_sequential_view(), propName);
		}
		else
		{
			rttr::variant wrapped_var = item.extract_wrapped_value();
			rttr::type value_type = wrapped_var.get_type();
			if (value_type.is_arithmetic() || value_type == rttr::type::get<std::string>() || value_type.is_enumeration())
			{
				reflectBasicType(value_type, wrapped_var, propName + "##" + std::to_string(i));
				view.set_value(i, wrapped_var);
			}
			else // object
			{
				//to_json_recursively(wrapped_var, writer);
			}
		}
		i++;
	}
}


void reflectProperty(rttr::property& prop, rttr::instance& obj)
{
	auto value_type = prop.get_type();
	auto wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
	bool is_wrapper = wrapped_type != value_type;


	//TODO: Fix wrapped types
	if (value_type.is_arithmetic() || value_type == rttr::type::get<std::string>() || value_type.is_enumeration())
	{
		rttr::variant var = prop.get_value(obj);
		var = is_wrapper ? var.extract_wrapped_value() : var;
		reflectBasicType(is_wrapper ? wrapped_type : value_type, var,
			prop.get_name().to_string());
		prop.set_value(obj, var);
	}
	//if(reflectBasicType(is_wrapper ? wrapped_type : value_type, obj, prop))
	//{
	//}
	else if (value_type == rttr::type::get<vec3f>())
	{
		rttr::variant var = prop.get_value(obj);
		reflectVec(var, obj, prop.get_name().to_string(), 3);
		prop.set_value(obj, var);
	}
	else if (value_type.is_sequential_container())
	{
		rttr::variant var = prop.get_value(obj);
		reflectArray(var.create_sequential_view(), prop.get_name().to_string());
		prop.set_value(obj, var);
	}
	else
	{
		auto child_props = is_wrapper ? wrapped_type.get_properties() : value_type.get_properties();
		if (!child_props.empty())
		{
			rttr::variant var = prop.get_value(obj);
			reflectUI(var);
			prop.set_value(obj, var);
		}

	}
}


bool reflectBasicType(const rttr::type& t, rttr::variant& var, NString propName)
{
	if (t.is_arithmetic())
	{
		if (t == rttr::type::get<bool>())
		{

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
			float value = var.to_float();
			if(ImGui::SliderFloat(propName.c_str(), &value, 100.0f, -100.f))
			{
				var = value;
			}
		}
		else if (t == rttr::type::get<double>())
		{

		}

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
		reflectProperty(prop, obj);
	}
}



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
		SceneViewSize = ImVec2(GlobalSettings::GetWindowWidth() / 2, GlobalSettings::GetWindowHeight() / 2);
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


	DrawInspector();
}

void EditorUI::DrawInspector()
{
	//ImGui::Begin("Inspector");
	//for (auto entity : SceneManager::currentScene.sceneObjects)
	//{
	//	
	//	ImGui::LabelText("mesh", entity->get<StaticMeshComponent>()->meshAssetFile.c_str());
	//	ECS::ComponentHandle<TransformComponent> transformComp = entity->get<TransformComponent>();
	//	reflectUI(transformComp.get());

	//}
	//ImGui::End();
	


	ImGui::Begin("Inspector");

	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_OpenOnDoubleClick;

	static int selection_mask = (1 << 2);
	int node_clicked = -1;
	int i = 0;
	for (auto entity : SceneManager::currentScene.sceneObjects)
	{
		// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
		ImGuiTreeNodeFlags node_flags = base_flags;
		const bool is_selected = (selection_mask & (1 << i)) != 0;
		if (is_selected)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		//RenderComponent* RC = ecs.GetComponent<RenderComponent>(entity);
		//if (RC)
		//{
			// Items 0..2 are Tree Node
		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, entity->get<StaticMeshComponent>()->meshAssetFile.c_str(), i);
		if (ImGui::IsItemClicked())
		{
			//    showInspector = true;
			node_clicked = i;
			//   selectedEntity = entity;
		}

		if (node_open)
		{
			ECS::ComponentHandle<TransformComponent> transformComp = entity->get<TransformComponent>();
			reflectUI(transformComp.get());

			ImGui::TreePop();
		}
		//}
		i++;
	}
	if (node_clicked != -1)
	{
		// Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
		if (ImGui::GetIO().KeyCtrl)
			selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
		else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
			selection_mask = (1 << node_clicked);           // Click to single-select
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
