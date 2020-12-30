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

void fromjson_recursively(rttr::instance obj);


/////////////////////////////////////////////////////////////////////////////////////////

static void write_array_recursively(rttr::variant_sequential_view& view)
{
	////view.set_size(json_array_value.Size());
	//const rttr::type array_value_type = view.get_rank_type(1);

	//for (int i = 0; i < view.get_size(); ++i)
	//{
	//	auto& json_index_value = json_array_value[i];
	//	if (json_index_value.IsArray())
	//	{
	//		auto sub_array_view = view.get_value(i).create_sequential_view();
	//		write_array_recursively(sub_array_view);
	//	}
	//	else if (json_index_value.IsObject())
	//	{
	//		rttr::variant var_tmp = view.get_value(i);
	//		rttr::variant wrapped_var = var_tmp.extract_wrapped_value();
	//		fromjson_recursively(wrapped_var);
	//		view.set_value(i, wrapped_var);
	//	}
	//	else
	//	{
	//		//rttr::variant extracted_value = extract_basic_types(json_index_value);
	//		//if (extracted_value.convert(array_value_type))
	//		//	view.set_value(i, extracted_value);
	//	}
	//}
}


void fromjson_recursively(rttr::instance obj2)
{
	rttr::instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;
	const auto prop_list = obj.get_derived_type().get_properties();

	for (auto prop : prop_list)
	{
		//Value::MemberIterator ret = json_object.FindMember(prop.get_name().data());
		//if (ret == json_object.MemberEnd())
		//	continue;
		const rttr::type value_t = prop.get_type();



		rttr::variant var;
		if (value_t.is_sequential_container())
		{
			var = prop.get_value(obj);
			auto view = var.create_sequential_view();
			write_array_recursively(view);
		}
		else
		{
			rttr::variant var = prop.get_value(obj);
			fromjson_recursively(var);
			prop.set_value(obj, var);
		}

		//auto& json_value = ret->value;
		//switch (json_value.GetType())
		//{
		//case kArrayType:
		//{
		//
		//	//else if (value_t.is_associative_container())
		//	//{
		//	//	var = prop.get_value(obj);
		//	//	auto associative_view = var.create_associative_view();
		//	//	write_associative_view_recursively(associative_view, json_value);
		//	//}
		//
		//	prop.set_value(obj, var);
		//	break;
		//}
		//case kObjectType:
		//{
		//	variant var = prop.get_value(obj);
		//	fromjson_recursively(var, json_value);
		//	prop.set_value(obj, var);
		//	break;
		//}
		//default:
		//{
		//	variant extracted_value = extract_basic_types(json_value);
		//	if (extracted_value.convert(value_t)) // REMARK: CONVERSION WORKS ONLY WITH "const type", check whether this is correct or not!
		//		prop.set_value(obj, extracted_value);
		//}
		//}
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
	ImGui::Begin("Inspector");
	//for (auto entity : SceneManager::currentScene.sceneObjects)
	//{
	//	ECS::ComponentHandle<TransformComponent> transformComp = entity->get<TransformComponent>();
	//	auto props = rttr::type::get(transformComp.get()).get_properties();
	//	to_ui_rec(transformComp.get());

	//}
	auto entity = SceneManager::currentScene.sceneObjects[0];
	ECS::ComponentHandle<TransformComponent> transformComp = entity->get<TransformComponent>();
	//fromjson_recursively(transformComp.get());
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
