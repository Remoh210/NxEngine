
#include "Application.h"
#include <Core/Camera/Camera.h>
#include <Common/CommonTypes.h>

#include <Core/Components/TransformComponent.h>
#include <Core/Systems/RenderSystem.h>
#include <Core/Systems/LineRenderSystem.h>
#include <Core/FileSystem/FileSystem.h>

#include <rendering/Sampler.h>
#include <rendering/RenderDevice.h>
#include <rendering/RenderTarget.h>
#include <rendering/AssetLoader.h>
#include <rendering/Shader.h>
#include <Editor/EditorRenderContext.h>

//Standard includes
#include <fstream>
#include <sstream>
#include <experimental/filesystem>


String TEST_TEXTURE_FILE     = Nx::FileSystem::GetPath("res/textures/stmpnk.jpg");
String SHADER_TEXT_FILE      = Nx::FileSystem::GetPath("res/shaders/basicShader.glsl");
String LINE_SHADER_TEXT_FILE = Nx::FileSystem::GetPath("res/shaders/LineShader.glsl");
String TEST_MODEL_FILE       = Nx::FileSystem::GetPath("res/models/monkey3.obj");
String TEST_MODEL_FILE2      = Nx::FileSystem::GetPath("res/models/rock/rock.obj");
String TEST_TEXTURE_FILE2    = Nx::FileSystem::GetPath("res/models/rock/rock.png");


// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

uint32 Application::numInstances = 0;

GLint TestTexId;

float lastX = 1200 / 2.0f;
float lastY = 700 / 2.0f;
bool firstMouse = true;

double Application::gScrollOffset = 0;
double Application::gXoffset = 0;
double Application::gYoffset = 0;
Camera* Application::MainCamera = nullptr;

//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void resize_callback(GLFWwindow * window, int width, int height);
void key_callback(GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mods);





Application* Application::Create(float Width, float Height)
{
	return new Application(Width, Height);
}

int Application::Run()
{
	Window window(SCR_WIDTH, SCR_HEIGHT, "Test!");

	window.SetMouseCallback
	(
		[this] (int xpos, int ypos)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		Application::GetMainCamera()->ProcessMouseMovement(xoffset, yoffset);
	}
	);


	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window.GetWindowHandle(), true);
	ImGui_ImplOpenGL3_Init("#version 410");
	ImGui::StyleColorsDark();

	//cSceneManager::LoadScene("test", ecs, *GetMainCamera());


	uint32 fps = 0;
	double currentFrame = glfwGetTime();
	double lastFrame = 0;
	double fpsTimeCounter = 0.0;
	double msPerFrame = 0;
	double msPerFrameToShow = 0;
	uint32 fpsToShow = 0;



	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	bool p_open = true;


	//Testing Texture
	RenderDevice renderDevice(window);
	Sampler sampler(renderDevice, SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR);


	int ha = window.GetHeight();
	int wa = window.GetWidth();
	mat4 projection = glm::perspective(glm::radians(45.0f), (float)window.GetWidth() / (float)window.GetHeight(), 0.1f, 10000.0f);

	DrawParams drawParams;
	drawParams.primitiveType = PRIMITIVE_TRIANGLES;
	drawParams.faceCulling = FACE_CULL_FRONT;
	drawParams.shouldWriteDepth = true;
	drawParams.depthFunc = DRAW_FUNC_LESS;
	//	drawParams.sourceBlend = RenderDevice::BLEND_FUNC_ONE;
	//	drawParams.destBlend = RenderDevice::BLEND_FUNC_ONE;

	RenderTarget target(renderDevice);
    EditorRenderContext EditorContext(renderDevice, target, drawParams, sampler, projection, MainCamera);

	//ECS
	ECS ecs;

	//model 1
	Array<IndexedModel> models;
	Array<uint32> modelMaterialIndices;
	Array<Material> modelMaterials;
	AssetLoader::LoadModels(TEST_MODEL_FILE, models, modelMaterialIndices, modelMaterials);
	VertexArray vertexArray(renderDevice, models[0], USAGE_STATIC_DRAW);

	ArrayBitmap testBitmap;
	testBitmap.Load(TEST_TEXTURE_FILE);
	Texture testtex(renderDevice, testBitmap, PixelFormat::FORMAT_RGBA, false, false);

	uint32 dbgTex = AssetLoader::TextureFromFile(TEST_TEXTURE_FILE);




	RenderableMeshComponent renderableMesh;
	renderableMesh.vertexArray = &vertexArray;
	renderableMesh.texture = &testtex;
	TransformComponent transformComp;
	transformComp.transform.position = vec3(0.9f, -0.15f, -40.0f);
	//transformComp.transform.rotation = vec3(5.9f, -0.15f, -50.0f);
	transformComp.transform.scale = vec3(7.0f);

	ecs.MakeEntity(transformComp, renderableMesh);


	//model2 
	AssetLoader::LoadModel(TEST_MODEL_FILE2, models, modelMaterialIndices, modelMaterials);
	VertexArray vertexArray2(renderDevice, models[1], USAGE_STATIC_DRAW);

	ArrayBitmap testBitmap2;
	testBitmap2.Load(modelMaterials[0].textureNames["texture_diffuse"]);
	Texture testtex2(renderDevice, testBitmap2, PixelFormat::FORMAT_RGBA, false, false);

	RenderableMeshComponent renderableMesh2;
	renderableMesh2.vertexArray = &vertexArray2;
	renderableMesh2.texture = &testtex2;
	renderableMesh2.numInst = 500;
	TransformComponent transformComp2;
	//transformComp.transform.position = vec3(0.9f, -0.15f, -40.0f);
	//transformComp.transform.rotation = vec3(5.9f, -0.15f, -50.0f);
	transformComp2.transform.scale = vec3(7.0f);

	ecs.MakeEntity(transformComp2, renderableMesh2);



	RenderableMeshSystem renderSystem(EditorContext, ecs);
	SystemList systemList;
	systemList.AddSystem(renderSystem);
  
	//Line Renderer
    Array<vec3> points;
	points.push_back(vec3(-0.5f, -0.5f, 0.0f));
	points.push_back(vec3(0.5f, -0.5f, 0.0f));
	points.push_back(vec3(0.0f, 0.5f, 0.0f));
	LineRenderer GridLineRenderer;
	VertexArray vertexArrayGRID(renderDevice, GridLineRenderer.CreateVertexArray(points), USAGE_STATIC_DRAW);


	LineRenderSystem lineRenderSystem(ecs);
	systemList.AddSystem(lineRenderSystem);

	RenderableMeshComponent LineRenderComp;
	LineRenderComp.vertexArray = &vertexArrayGRID;
	//LineRenderComp.numVertecies = points.size();
	//LineRenderComp.shader = &Line_shader;
	TransformComponent transformComp3;
	transformComp3.transform.position = vec3(0.9f, -0.15f, -40.0f);
	transformComp3.transform.rotation = vec3(5.9f, -0.15f, -50.0f);
	transformComp3.transform.scale = vec3(17.0f);



    //Load and set shaders
    String LineShaderText;
    loadTextFileWithIncludes(LineShaderText, LINE_SHADER_TEXT_FILE, "#include");
    Shader Line_shader(renderDevice, LineShaderText);

    String shaderText;
    loadTextFileWithIncludes(shaderText, SHADER_TEXT_FILE, "#include");
    Shader shader(renderDevice, shaderText);


    vertexArray.SetShader(&shader);
    vertexArray2.SetShader(&shader);
    vertexArrayGRID.SetShader(&Line_shader);

	ecs.MakeEntity(transformComp3, LineRenderComp);


	String TestString = Nx::FileSystem::GetPath("res/models/rock/rock.obj");

	DEBUG_LOG("TEST", "TEST", "MeshPath: %s", TestString.c_str());

	while (!window.ShouldClose())
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();



		ImGui::NewFrame();

		ImVec2 VecScreen(windowWidth/5, windowHeight/5);
		ImVec2 VecPos(0, 0);
		ImGui::SetNextWindowSize(VecScreen);
		ImGui::SetNextWindowPos(VecPos);



		ImGuiWindowFlags window_flags = 0;
		//window_flags |= ImGuiWindowFlags_NoTitleBar;
		//if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
		window_flags |= ImGuiWindowFlags_MenuBar;
		//window_flags |= ImGuiWindowFlags_NoMove;
		//window_flags |= ImGuiWindowFlags_NoResize;
		//window_flags |= ImGuiWindowFlags_NoCollapse;
		//if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
		//if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
		//if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		//if (no_close)           p_open = NULL; // Don't pass our bool* to Begin


		ImGui::Begin("Main Window", &p_open, window_flags);                          // Create a window called "Hello, world!" and append into it.
		GUI_ShowMenuBar(ecs);

		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		fpsTimeCounter += deltaTime;
		fps++;
		ImGui::Text("%f ms (%d fps)", msPerFrameToShow, fpsToShow);
		if (fpsTimeCounter >= 1.0) {
			fpsToShow = fps;
			double msPerFrame = 1000.0 / (double)fps;
			msPerFrameToShow = msPerFrame;
			fpsTimeCounter = 0;
			fps = 0;
		}


		//// Scene Tree
		//{
		//	ShowSceneObjectList(ecs);
		//}
		//ImGui::SameLine();
		//Inspector
		{
			ShowInspector(ecs);
		}
		//ImGui::NewLine();
		////Content Window
		//{
		//	ShowContentWindow(ecs);
		//}


		//// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
		{
			static float f = 0.0f;
			static int counter = 0;
			ImGui::Begin("Settings");                          // Create a window called "Hello, world!" and append into it.

			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		ImGui::End();



		// input
		// -----
		processInput(window.GetWindowHandle());

		//// Start the Dear ImGui frame
		//ImGui_ImplOpenGL3_NewFrame();
		//ImGui_ImplGlfw_NewFrame();
		////ImGui::NewFrame();

		//ImGui::ShowDemoWindow();

		ImVec2 sizeP(250, 350);
		ImVec2 sizeW(400, 400);
		ImGui::SetNextWindowSize(sizeW);
		ImGui::Begin("TestImageWindow");
		ImGui::Image((void*)testtex.GetId(), sizeP);
		ImGui::End();
		// render
		// ------
		EditorContext.Clear(glm::vec4(0.576, 0.439, 0.859, 0), true);

		//EditorContext.RenderMesh(vertexArray, testtex, trans);
		ecs.UpdateSystems(systemList, 0.0f);

		EditorContext.Flush();

		//



		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		//
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.Present();
		glfwPollEvents();
	}

	return 0;
}

Application::~Application()
{
}

bool Application::IsRunning()
{
	return isAppRunning;
}

Camera* Application::GetMainCamera()
{
	return MainCamera;
}

Application::Application(float Width, float Height)
{
	numInstances++;
	windowWidth = Width;
	windowHeight = Height;
	if (!MainCamera)
	{
		MainCamera = new Camera(glm::vec3(0.0f, 0.0f, 83.0f));
		MainCamera->bControlled = false;
		MainCamera->MovementSpeed = 100.0f;
	}
	
	
	deltaTime = 0;

}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Application::processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		MainCamera->ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		MainCamera->ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		MainCamera->ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		MainCamera->ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		MainCamera->ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		MainCamera->ProcessKeyboard(DOWN, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);


	if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
	{
		Application::MainCamera->bControlled = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
	{
		Application::MainCamera->bControlled = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

void Application::ResizeWindow(uint32 width, uint32 height)
{
	windowHeight = height;
	windowWidth = width;
}

void Application::DisplayEntyties(ECS& ecs)
{
	//HACK HACK HACK
	Array<EntityHandle> entities;
	entities = ecs.GetEntities();
	for (int i = 0; i < entities.size(); i++)
	{
		DEBUG_LOG_TEMP(ecs.GetComponent<RenderComponent>(entities[i])->mesh.c_str());
	}

}

void Application::loadEntityToScene(ECS& ecs, string name)
{
	//TransformComponent transformComponent;
	//transformComponent.transform.position = vec3f(0.f, 0.f, 0.f);
	//transformComponent.transform.scale = vec3f(1.f, 1.f, 1.f);
	//RenderComponent renderComponent;
	//renderComponent.diffuse = vec4f(0.2f, 0.4f, 0.5f, 1.f);
	//renderComponent.bIsVisible = true;
	//renderComponent.bIsWireFrame = true;
	//renderComponent.shader = "basic";
	//renderComponent.mesh = name;
	//renderComponent.meshPath = MapModelPaths.find(name)->second;

	//EntityHandle entity = ecs.MakeEntity(renderComponent, transformComponent);
	//cSceneManager::currentScene.sceneObjects.push_back(entity);
}

void Application::GUI_ShowMenuBar(ECS& ecs)
{
	// Menu Bar
	ImGui::BeginMenuBar();
	if (ImGui::BeginMenu("Menu"))
	{
		if (ImGui::MenuItem("New")) {}
		if (ImGui::MenuItem("Open", "Ctrl+O")) {}
		if (ImGui::MenuItem("Save Scene", "Ctrl+S")) 
		{                          // Buttons return true when clicked (most widgets return true when edited/activated)
			//cSceneManager::SaveScene("test", ecs, *GetMainCamera());
		}
		if (ImGui::MenuItem("Load Scene", "Ctrl+L"))
		{

			//cSceneManager::LoadScene("test", ecs, *GetMainCamera());
	
		}

		if (ImGui::MenuItem("Save As..")) {}
		ImGui::Separator();
		if (ImGui::BeginMenu("Options"))
		{
			static bool enabled = true;
			ImGui::MenuItem("Enabled", "", &enabled);
			ImGui::BeginChild("child", ImVec2(0, 60), true);
			for (int i = 0; i < 10; i++)
				ImGui::Text("Scrolling Text %d", i);
			ImGui::EndChild();
			static float f = 0.5f;
			static int n = 0;
			static bool b = true;
			ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
			ImGui::InputFloat("Input", &f, 0.1f);
			ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
			ImGui::Checkbox("Check", &b);
			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Examples"))
	{
		ImGui::MenuItem("Main menu bar", NULL/*, &show_app_main_menu_bar*/);
		ImGui::MenuItem("Console", NULL/*, &show_app_console*/);
		ImGui::MenuItem("Log", NULL/*, &show_app_log*/);
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("Tools"))
	{
		ImGui::MenuItem("Metrics", NULL/*, &show_app_metrics*/);
		ImGui::MenuItem("Style Editor", NULL/*, &show_app_style_editor*/);
		ImGui::MenuItem("About Dear ImGui", NULL/*, &show_app_about*/);
		ImGui::EndMenu();
	}
	ImGui::EndMenuBar();
}






void Application::ShowContentWindow(ECS& ecs)
{
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	ImGui::BeginChild("Content Window", ImVec2(0, GetPercentOf(30, windowHeight)), true/*, window_flags*/);

	ImGui::SetNextWindowContentSize(ImVec2(100 * MapModelPaths.size(), 0.0f));
	ImGui::BeginChild("##ScrollingRegion", ImVec2(0, ImGui::GetFontSize() * 20), false, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::Columns(MapModelPaths.size());
	int ITEMS_COUNT = 1;
	ImGuiListClipper clipper(ITEMS_COUNT);  // Also demonstrate using the clipper for large list
	while (clipper.Step())
	{
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
		{
			int j = 0;
			for (const auto& x : MapModelPaths)
			{

				ImGui::PushID(j);
				int frame_padding = -1 + i;     // -1 = uses default padding
				//ImGui::Image((void*)(intptr_t)TestTexId, ImVec2(400/5, 400/5));
				if (ImGui::ImageButton((void*)(intptr_t)TestTexId, ImVec2(400 / 5, 400 / 5)))
				{
					loadEntityToScene(ecs, x.first);
				}
				ImGui::PopID();
				ImGui::TextWrapped(x.first.c_str());
				ImGui::NextColumn();

				j++;
			}
		}
	}
	ImGui::Columns(1);
	ImGui::EndChild();

	ImGui::EndChild();
	ImGui::PopStyleVar();
}


bool showInspector = false;
EntityHandle selectedEntity;

void Application::ShowSceneObjectList(ECS& ecs)
{
	//ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	//ImGui::BeginChild("Scene Tree", ImVec2(GetPercentOf(20, windowWidth), GetPercentOf(70, windowHeight)), true/*, window_flags*/);
	//auto enties = ecs.GetEntities();

	//static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow 
	//	| ImGuiTreeNodeFlags_OpenOnDoubleClick 
	//	/*| ImGuiTreeNodeFlags_SpanAvailWidth*/;

	//static int selection_mask = (1 << 2);
	//int node_clicked = -1;
	//int i = 0;
	//for (const auto& entity : cSceneManager::currentScene.sceneObjects)
	//{
	//	// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
	//	ImGuiTreeNodeFlags node_flags = base_flags;
	//	const bool is_selected = (selection_mask & (1 << i)) != 0;
	//	if (is_selected)
	//		node_flags |= ImGuiTreeNodeFlags_Selected;

	//	RenderComponent* RC = ecs.GetComponent<RenderComponent>(entity);
	//	if (RC)
	//	{
	//		// Items 0..2 are Tree Node
	//		bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, RC->mesh.c_str(), i);
	//		if (ImGui::IsItemClicked())
	//		{
	//			showInspector = true;
	//			node_clicked = i;
	//			selectedEntity = entity;
	//		}
	//			
	//		if (node_open)
	//		{
	//			ImGui::BulletText("Child1"); 
	//			ImGui::BulletText("Child2");
	//			ImGui::TreePop();
	//		}
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
	//
	//ImGui::EndChild();
	//ImGui::PopStyleVar();

}

void Application::ShowInspector(ECS& ecs)
{
	//Inspector
	if (showInspector)
	{
		TransformComponent* TC = ecs.GetComponent<TransformComponent>(selectedEntity);
		if (TC)
		{
			//position
			float PosVec[3];
			for (int i = 0; i < 3; i++)
			{
				PosVec[i] = TC->transform.position[i];
			}
			//Scale
			float scale = TC->transform.scale.x;



			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 1.0f);
			ImGui::BeginChild("Inspector", ImVec2(GetPercentOf(20, windowWidth) - 45, GetPercentOf(70, windowHeight)), true/*, window_flags*/);

			ImGui::DragFloat3("Position", PosVec, 0.01f);
			ImGui::DragFloat("Scale", &scale, 0.01f, 0.001f, 1000.0f);

			//position
			for (int i = 0; i < 3; i++)
				TC->transform.position[i] = PosVec[i];
			//scale
			for (int i = 0; i < 3; i++)
				TC->transform.scale[i] = scale;


			if (ImGui::Button("Remove Object"))
			{
				//cSceneManager::RemoveObjectFromScene(selectedEntity, ecs);
				showInspector = false;
			}
			if (ImGui::Button("Close"))
			{
				showInspector = false;
			}
			ImGui::EndChild();
			ImGui::PopStyleVar();
		}


	}
}

void Application::ShowEditor(GLint EditorTex)
{
	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 1.0f);
	ImGui::BeginChild("ViewPort", ImVec2(GetPercentOf(60, windowWidth), GetPercentOf(70, windowHeight)), true/*, window_flags*/);

	ImGui::Image((void *)EditorTex, ImVec2(GetPercentOf(60, windowWidth), GetPercentOf(70, windowHeight)), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::EndChild();
	ImGui::PopStyleVar();
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void Application::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	gXoffset = xpos - lastX;
	gYoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;
	Application::GetMainCamera()->ProcessMouseMovement(gXoffset, gYoffset);
}

//// glfw: whenever the mouse scroll wheel scrolls, this callback is called
//// ----------------------------------------------------------------------
void Application::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	gScrollOffset = yoffset;
}


//// glfw: whenever the window size changed (by OS or user resize) this callback function executes
//// ---------------------------------------------------------------------------------------------
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//	// make sure the viewport matches the new window dimensions; note that width and 
//	// height will be significantly larger than specified on retina displays.
//	glViewport(0, 0, width, height);
//}


void resize_callback(GLFWwindow * window, int width, int height)
{
	Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
	app->ResizeWindow(width, height);
}


bool cursor = true;
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
	app->processInput(window);
	//LOAD MODELS
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		app->GetMainCamera()->ToggleControls();
		cursor = !cursor;
		glfwSetInputMode(window, GLFW_CURSOR, cursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
	}
	
}


bool Application::loadTextFileWithIncludes(String& output, const String& fileName,
		const String& includeKeyword)
{
	std::ifstream file;
	file.open(fileName.c_str());

	String filePath = StringFuncs::getFilePath(fileName);
	std::stringstream ss;
	String line;

	if(file.is_open()) {
		while(file.good()) {
			getline(file, line);
			
			if(line.find(includeKeyword) == String::npos) {
				ss << line << "\n";
			} else {
				String includeFileName = StringFuncs::split(line, ' ')[1];
				includeFileName =
					includeFileName.substr(1,includeFileName.length() - 2);

				String toAppend;
				loadTextFileWithIncludes(toAppend, filePath + includeFileName,
						includeKeyword);
				ss << toAppend << "\n";
			}
		}
	} else {
		DEBUG_LOG(LOG_TYPE_IO, LOG_ERROR, "Unable to load shader: %s",
				fileName.c_str());
		return false;
	}

	output = ss.str();
	return true;
}

