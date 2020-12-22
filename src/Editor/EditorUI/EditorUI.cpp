#include "EditorUI.h"

#include "imgui.h"
#include "Common/CommonTypes.h"
#include "Core/Application/SceneManager/SceneManager.h"
#include "Core/Application/Settings/GlobalSettings.h"
#include "Core/Components/TransformComponent.h"

bool EditorUI::init = true;
ImVec2 EditorUI::SceneViewSize;
ImVec2 EditorUI::SceneViewPos;

bool write_atomic_types_to_ui(const rttr::type& t, const rttr::variant& var);
static void write_array(const rttr::variant_sequential_view& view);
bool write_variant(const rttr::variant& var);
void to_ui_rec(const rttr::instance& obj2);
static void WriteVec(const rttr::variant_sequential_view& view, Array<rttr::variant>& varArray);

void WriteVec(const rttr::variant_sequential_view& view, Array<rttr::variant>& varArray)
{
	for (const auto& item : view)
	{
		rttr::variant wrapped_var = item.extract_wrapped_value();
		
		rttr::type value_type = wrapped_var.get_type();
		NString type = value_type.get_name().to_string();
		varArray.push_back(wrapped_var);
	}
}


bool write_atomic_types_to_ui(const rttr::type& t, const rttr::variant& var)
{
	if (t.is_arithmetic())
	{
		if (t == rttr::type::get<bool>())
			return true;
		else if (t == rttr::type::get<char>())
			return true;
		else if (t == rttr::type::get<int8_t>())
			return true;
		else if (t == rttr::type::get<int16_t>())
			return true;
		else if (t == rttr::type::get<int32_t>())
			return true;
		else if (t == rttr::type::get<int64_t>())
			return true;
		else if (t == rttr::type::get<uint8_t>())
			return true;
		else if (t == rttr::type::get<uint16_t>())
			return true;
		else if (t == rttr::type::get<uint32_t>())
			return true;
		else if (t == rttr::type::get<uint64_t>())
			return true;
		else if (t == rttr::type::get<float>())
		{
			//float tst = var.to_float();
			//ImGui::DragFloat("test", &tst, 0.2f, -10000.0f, 10000.0f);
			//return true;
		}
		else if (t == rttr::type::get<double>())
			return true;
		return true;
	}
	//else if (t.is_enumeration())
	//{
	//	bool ok = false;
	//	auto result = var.to_string(&ok);
	//	if (ok)
	//	{
	//		writer.String(var.to_string());
	//	}
	//	else
	//	{
	//		ok = false;
	//		auto value = var.to_uint64(&ok);
	//		if (ok)
	//			writer.Uint64(value);
	//		else
	//			writer.Null();
	//	}

	//	return true;
	//}
	else if (t == rttr::type::get<std::string>())
	{
		return true;
	}

	return false;
}

static void write_array(const rttr::variant_sequential_view& view)
{
	for (const auto& item : view)
	{
		if (item.is_sequential_container())
		{
			write_array(item.create_sequential_view());
		}
		else
		{
			rttr::variant wrapped_var = item.extract_wrapped_value();
			rttr::type value_type = wrapped_var.get_type();
			if (value_type.is_arithmetic() || value_type == rttr::type::get<std::string>() || value_type.is_enumeration())
			{
				write_atomic_types_to_ui(value_type, wrapped_var);
			}
			else // object
			{
				to_ui_rec(wrapped_var);
			}
		}
	}
}

float floatArray[3] = { 1,
		2,
		3
};

bool write_variant(const rttr::variant& var, const rttr::string_view& propertyName)
{
	auto value_type = var.get_type();
	auto wrapped_type = value_type.is_wrapper() ? value_type.get_wrapped_type() : value_type;
	bool is_wrapper = wrapped_type != value_type;

	//if (write_atomic_types_to_json(is_wrapper ? wrapped_type : value_type,
	//	is_wrapper ? var.extract_wrapped_value() : var, writer))
	//{
	//}
	NString testStrimg = value_type.get_name().to_string();


	if (value_type == rttr::type::get<float>())
	{
		prop.set_value(obj, 12.f);
		float test = var.to_float();
		ImGui::DragFloat(propertyName.to_string().c_str(), &test, 0.2, -1000, 1000);
	}

	if (value_type == rttr::type::get<vec3f>())
	{
		const rttr::instance& obj2 = var;

		rttr::instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;

		auto prop_list = obj.get_derived_type().get_properties();
		for (auto prop : prop_list)
		{

			


			Array<rttr::variant> arrayToWrite;
			rttr::variant prop_value = prop.get_value(obj);
			WriteVec(prop_value.create_sequential_view(), arrayToWrite);

			if (arrayToWrite.size() == 3)
			{
				//float floatArray[3] = { arrayToWrite[0].to_float(),
				//		arrayToWrite[1].to_float(),
				//		arrayToWrite[2].to_float()
				//};
				//const rttr::property propTest = var;
				if(ImGui::DragFloat3(propertyName.to_string().c_str(), floatArray, 0.2, -1000, 1000))
				{

					auto view = prop_value.create_sequential_view();
					for (int i = 0; i < 3; i++)
					{
						
						if (view.set_value(i, floatArray[i]))
						{
							DEBUG_LOG("TEST", "TEST", "SUCCESS");
						}

					}
					prop.set_value(obj, view);

				}
				
			}
			
		}

		

	}
	if (var.is_sequential_container())
	{
		write_array(var.create_sequential_view());
	}
	//else if (var.is_associative_container())
	//{
	//	write_associative_container(var.create_associative_view(), writer);
	//}
	else
	{
		auto child_props = is_wrapper ? wrapped_type.get_properties() : value_type.get_properties();
		if (!child_props.empty())
		{
			to_ui_rec(var);
		}
		else
		{
			bool ok = false;
			auto text = var.to_string(&ok);
			if (!ok)
			{
				return false;
			}
		}
	}

	return true;
}

void to_ui_rec(const rttr::instance& obj2)
{
	rttr::instance obj = obj2.get_type().get_raw_type().is_wrapper() ? obj2.get_wrapped_instance() : obj2;

	auto prop_list = obj.get_derived_type().get_properties();
	for (auto prop : prop_list)
	{
		if (prop.get_metadata("NO_SERIALIZE"))
			continue;

		rttr::variant prop_value = prop.get_value(obj);
		if (!prop_value)
			continue; // cannot serialize, because we cannot retrieve the value

		const auto name = prop.get_name();
		if (!write_variant(prop_value, name))
		{
			std::cerr << "cannot serialize property: " << name << std::endl;
		}
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
	to_ui_rec(transformComp.get());
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
