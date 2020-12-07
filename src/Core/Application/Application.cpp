
#include "Application.h"
#include <Core/Camera/Camera.h>
#include <Common/CommonTypes.h>

#include <Core/Components/TransformComponent.h>
#include <Core/Components/LightComponent.h>
#include <Core/Systems/RenderSystem.h>
#include <Core/FileSystem/FileSystem.h>
#include <Core/Graphics/DebugRenderer/DebugRenderer.h>
#include <Core/Graphics/Cubemap/CubemapManager.h>
#include <Core/Application/SceneManager/SceneManager.h>
#include <Core/Graphics/ShaderManager/ShaderManager.h>

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


// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1000;

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
	Initialize();

	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui_ImplGlfw_InitForOpenGL(window->GetWindowHandle(), true);
	ImGui_ImplOpenGL3_Init("#version 410");
	ImGui::StyleColorsDark();


	bool show_demo_window = false;
	bool show_another_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	bool p_open = true;

	//UI

	uint32 fps = 0;
	double currentFrame = glfwGetTime();
	double lastFrame = 0;
	double fpsTimeCounter = 0.0;
	double msPerFrame = 0;
	double msPerFrameToShow = 0;
	uint32 fpsToShow = 0;


    //TODO: Fix this
	AssetLoader::SetShouldFlipVTexture(true);


	ECS::EntitySystem* renderSystem = world->registerSystem(new ECS::RenderableMeshSystem(editorRenderContext));

	DebugRenderer debugRenderer(*editorRenderContext);
	for (int i = 0; i < 1; i++)
	{
		//debugRenderer.DrawDebugSphere(vec3(0.f), 10, 50, vec3(1, 0, 0));
	}
	
	//debugRenderer.DrawDebugLine(vec3(0.f), vec3(0.0f, 30.0f, 0.0f), 5, vec3(0, 1, 0));
	vec3 debugSpherePos(0.0f);


	LoadDefaultScene();

	Array<glm::vec3> lightPositions;
	lightPositions.push_back(glm::vec3(-10.0f, 10.0f, 10.0f));
	lightPositions.push_back(glm::vec3(10.0f, 10.0f, 10.0f));
	lightPositions.push_back(glm::vec3(-10.0f, -10.0f, 10.0f));
	lightPositions.push_back(glm::vec3(10.0f, -10.0f, 10.0f));

	while (!window->ShouldClose())
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImVec2 VecScreen(windowWidth/5, windowHeight/5);
		ImVec2 VecPosRel = ImGui::GetMainViewport()->Pos;
		ImVec2 VecPos(150, 150);
		ImGui::SetNextWindowSize(VecScreen);
		ImGui::SetNextWindowPos(VecPosRel);
		ImGuiWindowFlags window_flags = 0;
		//window_flags |= ImGuiWindowFlags_NoTitleBar;
		//if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
		window_flags |= ImGuiWindowFlags_MenuBar;
		window_flags |= ImGuiWindowFlags_NoDocking;
		//window_flags |= ImGuiWindowFlags_NoMove;
		//window_flags |= ImGuiWindowFlags_NoResize;
		//window_flags |= ImGuiWindowFlags_NoCollapse;
		//if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
		//if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
		//if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
		//if (no_close)           p_open = NULL; // Don't pass our bool* to Begin


		ImGui::Begin("Main Window", &p_open, window_flags);
		GUI_ShowMenuBar();

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
		ImGui::Text("Camera Speed: %.1f)", MainCamera->MovementSpeed);

		if (ImGui::Button("Show demo"))
		{
			show_demo_window = true;
		}

		if (ImGui::Button("Draw Grid"))
		{
			editorRenderContext->ToggleGrid();
		}

		auto staticMesh = SceneManager::currentScene.sceneObjects[0]->get<StaticMeshComponent>();
		
		ImGui::SliderFloat("roughness", &staticMesh->meshes[0]->material->roughness, 0.0f, 1.0f);
		ImGui::SliderFloat("metallic", &staticMesh->meshes[0]->material->metallic, 0.0f, 1.0f);
		
		ImGui::End();


		//// Scene Tree
		//{
		//	ShowSceneObjectList(ecs);
		//}
		//ImGui::SameLine();
		//Inspector
		{
			ShowInspector();
		}
		//ImGui::NewLine();
		////Content Window
		//{
		//	ShowContentWindow(ecs);
		//}


		//// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		for (int i = 0; i < 1; i++)
		{
			debugSpherePos.x += 0.1f * deltaTime;
			//debugRenderer.DrawDebugSphere(debugSpherePos, 0, 10, vec3(0, 1, 0));
		}

		// input
		// -----
		
		//// Start the Dear ImGui frame
		//ImGui_ImplOpenGL3_NewFrame();
		//ImGui_ImplGlfw_NewFrame();
		////ImGui::NewFrame();

		//ImGui::ShowDemoWindow();

		//ImVec2 sizeP(250, 350);
		//ImVec2 sizeW(400, 400);
		//ImGui::SetNextWindowSize(sizeW);
		//ImGui::Begin("TestImageWindow");
		//ImGui::Image((void*)testtex.GetId(), sizeP);
		//ImGui::End();


		for (glm::vec3 pos : lightPositions)
		{
			debugRenderer.DrawDebugSphere(pos, 0.0f, 1.f);
		}

		// render
		// ------
		editorRenderContext->Clear(glm::vec4(0.34, 0.3, 0.5, 0), true);
		debugRenderer.Update(deltaTime);
		world->tick(deltaTime);
		editorRenderContext->Flush();


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		processInput(window->GetWindowHandle());
		glfwPollEvents();
		window->Present();

		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}

	//Delete
	ShutDown();

	// UI Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//glfwDestroyWindow(window);
	glfwTerminate();

	

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
		MainCamera = new Camera(glm::vec3(0.0f, 10.0f, 80.0f));
		MainCamera->bControlled = false;
		MainCamera->MovementSpeed = 50.0f;
	}
		
	deltaTime = 0;
}


void Application::Initialize()
{
	window = new Window(SCR_WIDTH, SCR_HEIGHT, "Test!");
	//TODO: Put to init UI
	window->SetMouseCallback
	(
		[this](int xpos, int ypos)
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
	window->SetMouseScrollCallback
	(
		[this](float yoffset)
	{
		Application::GetMainCamera()->ProcessMouseScroll(yoffset);
	}
	);

	renderDevice = new RenderDevice(window);

	
	//Init Shaders**********************************************************

	//TODO: Move to cPipeline?
	NString PBR_SHADER_TEXT_FILE = "res/shaders/PBR/pbr.glsl";
	NString PBRshaderText;
	loadTextFileWithIncludes(PBRshaderText, PBR_SHADER_TEXT_FILE, "#include");
	Shader* PBRshader = new Shader(renderDevice, PBRshaderText);
	ShaderManager::SetMainShader(PBRshader);
	ShaderManager::AddPBRShader("PBR_SHADER", PBRshader);


	NString EQ_TO_CUBEMAP_SHADER_TEXT_FILE = "res/shaders/PBR/eqToCube.glsl";
	NString eqToCubeshaderText;
	loadTextFileWithIncludes(eqToCubeshaderText, EQ_TO_CUBEMAP_SHADER_TEXT_FILE, "#include");
	Shader* PBREqToCubeshader = new Shader(renderDevice, eqToCubeshaderText);
	ShaderManager::AddPBRShader("EQ_TO_CUBE_SHADER", PBREqToCubeshader);

	NString PBR_IR_CONV_SHADER_TEXT_FILE = "res/shaders/PBR/irConv.glsl";
	NString IrConvShaderText;
	loadTextFileWithIncludes(IrConvShaderText, PBR_IR_CONV_SHADER_TEXT_FILE, "#include");
	Shader* IrConvShader = new Shader(renderDevice, IrConvShaderText);
	ShaderManager::AddPBRShader("IR_CONV_SHADER", IrConvShader);

	NString PREFILTER_SHADER_TEXT_FILE = "res/shaders/PBR/prefilter.glsl";
	NString PrefilterShaderText;
	loadTextFileWithIncludes(PrefilterShaderText, PREFILTER_SHADER_TEXT_FILE, "#include");
	Shader* PrefilterShader = new Shader(renderDevice, PrefilterShaderText);
	ShaderManager::AddPBRShader("PREFILTER_SHADER", PrefilterShader);

	NString BRDF_SHADER_TEXT_FILE = "res/shaders/PBR/brdf.glsl";
	NString brdfShaderText;
	loadTextFileWithIncludes(brdfShaderText, BRDF_SHADER_TEXT_FILE, "#include");
	Shader* brdfShader = new Shader(renderDevice, brdfShaderText);
	ShaderManager::AddPBRShader("BRDF_SHADER", brdfShader);

	NString SKYBOX_SHADER_TEXT_FILE = "res/shaders/PBR/skybox.glsl";
	NString SkyBoxShaderText;
	loadTextFileWithIncludes(SkyBoxShaderText, SKYBOX_SHADER_TEXT_FILE, "#include");
	Shader* SkyboxShader = new Shader(renderDevice, SkyBoxShaderText);
	ShaderManager::AddPBRShader("SKYBOX_SHADER", SkyboxShader);

	//Init Shaders**********************************************************

	editorSampler = new Sampler(renderDevice, SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR);
	DrawParams drawParams;
	drawParams.primitiveType = PRIMITIVE_TRIANGLES;
	drawParams.faceCulling = FACE_CULL_NONE;
	drawParams.shouldWriteDepth = true;
	drawParams.depthFunc = DRAW_FUNC_LESS;
	//drawParams.sourceBlend = BLEND_FUNC_SRC_ALPHA;
	//drawParams.destBlend = BLEND_FUNC_ONE;

	int ha = window->GetHeight();
	int wa = window->GetWidth();
	mat4 projection = glm::perspective(glm::radians(45.0f), (float)window->GetWidth() / (float)window->GetHeight(), 0.1f, 10000.0f);

	CubemapManager::Inititialize(renderDevice, PBREqToCubeshader, IrConvShader);
	editorRenderTarget = new RenderTarget(renderDevice);
	editorRenderContext = new EditorRenderContext(renderDevice, editorRenderTarget, drawParams, editorSampler, projection, MainCamera);


	SceneManager::SetECS(world);
	SceneManager::SetRenderDevice(renderDevice);
	ShaderManager::SetRenderDevice(renderDevice);
	
	


	//ECS
	world = ECS::World::createWorld();
}

void Application::ShutDown()
{
	world->destroyWorld();
	delete renderDevice;
	delete editorSampler;
	delete editorRenderTarget;
	delete editorRenderContext;
}

void Application::LoadDefaultScene()
{
	NString monkeyMesh = "res/models/monkey3.obj";
	NString rockMesh = "res/models/rock/rock.obj";

	NString TEST_TEXTURE_FILE = "res/textures/stmpnk.jpg";

	NString TEST_MODEL_FILE = monkeyMesh;
	NString TEST_MODEL_FILE2 = rockMesh;


	//Assimp PBR********************************************************
	//PBRSphere.glb = binary and embedded textures
	//PBRSphere2.gltf = embedded textures
	NString TEST_MODEL_FILE3 = "res/models/pistol_test.glb"; 
	//Assimp PBR********************************************************
	


	NString TEST_TEXTURE_FILE2 = "res/models/rock/rock.png";

	//Manual texure loading 
	NString TexureType = "rusted_iron";
	NString TexurePathBase = "res/textures/pbr";
	NString file_albedoTex = TexurePathBase + "/" + TexureType + "/" + "albedo.png";
	NString file_normalTex = TexurePathBase + "/" + TexureType + "/" + "normal.png";
	NString file_metallicTex = TexurePathBase + "/" + TexureType + "/" + "metallic.png";
	NString file_roughnessTex = TexurePathBase + "/" + TexureType + "/" + "roughness.png";
	NString file_aoTex = TexurePathBase + "/" + TexureType + "/" + "ao.png";

	ArrayBitmap albedoBitMap;
	albedoBitMap.Load(file_albedoTex);
	Texture* albedo = new Texture(renderDevice, albedoBitMap, PixelFormat::FORMAT_RGBA, true, false);

	ArrayBitmap normalBitMap;
	normalBitMap.Load(file_normalTex);
	Texture* normal = new Texture(renderDevice, normalBitMap, PixelFormat::FORMAT_RGBA, true, false);

	ArrayBitmap metallicBitMap;
	metallicBitMap.Load(file_metallicTex);
	Texture* metallic = new Texture(renderDevice, metallicBitMap, PixelFormat::FORMAT_RGBA, true, false);

	ArrayBitmap roughnessBitMap;
	roughnessBitMap.Load(file_roughnessTex);
	Texture* roughness = new Texture(renderDevice, roughnessBitMap, PixelFormat::FORMAT_RGBA, true, false);

	ArrayBitmap aoBitMap;
	aoBitMap.Load(file_aoTex);
	Texture* ao = new Texture(renderDevice, aoBitMap, PixelFormat::FORMAT_RGBA, true, false);

	//model 1
	Array<IndexedModel> models;
	Array<uint32> modelMaterialIndices;
	Array<MaterialSpec> modelMaterials;
	AssetLoader::LoadModel(TEST_MODEL_FILE, models, modelMaterialIndices, modelMaterials);
	//VertexArray vertexArray(renderDevice, models[0], USAGE_STATIC_DRAW);
	VertexArray* vertexArray = new VertexArray(renderDevice, models[0], USAGE_STATIC_DRAW);

	ArrayBitmap testBitmap;
	testBitmap.Load(TEST_TEXTURE_FILE);
	Texture* testtex = new Texture(renderDevice, testBitmap, PixelFormat::FORMAT_RGBA, false, false);


	MeshInfo* meshInfo1 = new MeshInfo();
	meshInfo1->vertexArray = vertexArray;
	Material* material1 = new Material();
	material1->textures[TEXTURE_ALBEDO] = testtex;
	meshInfo1->material = material1;
	StaticMeshComponent renderableMesh;
	renderableMesh.meshAssetFile = monkeyMesh;
	renderableMesh.shader = ShaderManager::GetMainShader();
	renderableMesh.meshes.Add(meshInfo1);
	TransformComponent transformComp;
	transformComp.transform.position = vec3(0.9f, 222.15f, -40.0f);
	transformComp.transform.scale = vec3(7.0f);

	//model2 
	AssetLoader::LoadModel(TEST_MODEL_FILE2, models, modelMaterialIndices, modelMaterials);
	VertexArray* vertexArray2 = new VertexArray(renderDevice, models[1], USAGE_STATIC_DRAW);
	//VertexArray vertexArray2(renderDevice, models[1], USAGE_STATIC_DRAW);

	ArrayBitmap testBitmap2;
	testBitmap2.Load(modelMaterials[1].textureNames[TEXTURE_ALBEDO]);
	Texture* testtex2 = new Texture (renderDevice, testBitmap2, PixelFormat::FORMAT_RGBA, false, false);

	MeshInfo* meshInfo2 = new MeshInfo;
	meshInfo2->vertexArray = vertexArray2;
	Material* material2 = new Material();
	material2->textures[TEXTURE_ALBEDO] = testtex2;
	//meshInfo2->material = material2;
	StaticMeshComponent renderableMesh2;
	renderableMesh2.meshAssetFile = rockMesh;
	renderableMesh2.shader = ShaderManager::GetMainShader();
	renderableMesh2.meshes.Add(meshInfo2);
	renderableMesh2.numInst = 100;
	TransformComponent transformComp2;
	transformComp2.transform.scale = vec3(7.0f);

	Array<IndexedModel> PBRLoadedMeshes;
	Array<uint32> PBRMaterialIndices;
	Array<MaterialSpec> PBRLoadedMaterials;
	AssetLoader::LoadModel(TEST_MODEL_FILE3, PBRLoadedMeshes, PBRMaterialIndices, PBRLoadedMaterials);


	StaticMeshComponent renderableMesh3;
	
	for(int i = 0; i < PBRLoadedMeshes.size(); i++)
	{
		MeshInfo* curMesh = new MeshInfo();
		curMesh->vertexArray = new VertexArray(renderDevice, PBRLoadedMeshes[i], USAGE_STATIC_DRAW);
		renderableMesh3.meshes.Add(curMesh);

		Material* material3 = new Material();
		for (int j = 0; j < PBRLoadedMaterials.size(); j++)
		{
			for (auto textureTypeToFile : PBRLoadedMaterials[i].textureNames)
			{
				ArrayBitmap bm;
				bm.Load(textureTypeToFile.second);
				Texture* tex = new Texture(renderDevice, bm, PixelFormat::FORMAT_RGBA, false, false);
				material3->textures[textureTypeToFile.first] = tex;
			}
			for (auto textureTypeToTex : PBRLoadedMaterials[i].textures)
			{
				material3->textures[textureTypeToTex.first] = textureTypeToTex.second;
			}
		}
		curMesh->material = material3;
	}

	renderableMesh3.shader = ShaderManager::GetMainShader();
	TransformComponent transformComp3;
	//transformComp3.transform.position = vec3(0.0f, 5.0f, -30.0f);
	transformComp3.transform.position = vec3(15.1f, 0.0f, -40.0f);
	transformComp3.transform.rotation = vec3(0.0, 0.0f, 0.f);
	transformComp3.transform.scale = vec3(0.5);

	MeshInfo* pbrTestMesh = new MeshInfo();
	pbrTestMesh->vertexArray = new VertexArray(renderDevice, PrimitiveGenerator::CreateSphere(1.0f, 36, 36, vec3(0.0f)), BufferUsage::USAGE_DYNAMIC_DRAW);
	Material* material4 = new Material();


	material4->textures[TEXTURE_ALBEDO] = albedo;
	material4->textures[TEXTURE_NORMAL] = normal;
	material4->textures[TEXTURE_METALLIC] = metallic;
	material4->textures[TEXTURE_ROUGHNESS] = roughness;
	material4->textures[TEXTURE_AO] = ao;

	//material3.diffuseTextures.Add(&testtex);
	pbrTestMesh->material = material4;
	StaticMeshComponent renderableMesh4;
	renderableMesh4.shader = ShaderManager::GetMainShader();
	renderableMesh4.meshes.Add(pbrTestMesh);
	TransformComponent transformComp4;
	transformComp4.transform.position = vec3(0, 0, -40);
	//transformComp.transform.rotation = vec3(5.9f, -0.15f, -50.0f);
	transformComp4.transform.scale = vec3(5);




	MeshInfo* mesh5 = new MeshInfo();
	mesh5->vertexArray = new VertexArray(renderDevice, PrimitiveGenerator::CreateSphere(1.0f, 36, 36, vec3(0.0f)), BufferUsage::USAGE_DYNAMIC_DRAW);
	Material* material5 = new Material();
	//material5->color(1.0);
	material5->metallic = 0.99;
	material5->roughness = 0.01;
	//material5->textures[TEXTURE_ALBEDO] = albedo;
	//material5->textures[TEXTURE_NORMAL] = normal;
	//material5->textures[TEXTURE_METALLIC] = metallic;
	//material5->textures[TEXTURE_ROUGHNESS] = roughness;
	//material5->textures[TEXTURE_AO] = ao;

	//material3.diffuseTextures.Add(&testtex);
	mesh5->material = material5;
	StaticMeshComponent renderableMesh5;
	renderableMesh5.shader = ShaderManager::GetMainShader();
	renderableMesh5.meshes.Add(mesh5);
	TransformComponent transformComp5;
	transformComp5.transform.position = vec3(0, 20, -40);
	//transformComp.transform.rotation = vec3(5.9f, -0.15f, -50.0f);
	transformComp5.transform.scale = vec3(5);


/*	ECS::Entity* ent = world->create();
	ent->assign<TransformComponent>(transformComp);
	ent->assign<StaticMeshComponent>(renderableMesh)*/;

	//ECS::Entity* ent2 = world->create();
	//ent2->assign<TransformComponent>(transformComp2);
	//ent2->assign<StaticMeshComponent>(renderableMesh2);

	ECS::Entity* ent3 = world->create();
	ent3->assign<TransformComponent>(transformComp3);
	ent3->assign<StaticMeshComponent>(renderableMesh3);

	ECS::Entity* ent4 = world->create();
	ent4->assign<TransformComponent>(transformComp4);
	ent4->assign<StaticMeshComponent>(renderableMesh4);

	ECS::Entity* ent5 = world->create();
	ent5->assign<TransformComponent>(transformComp5);
	ent5->assign<StaticMeshComponent>(renderableMesh5);

	//SceneManager::currentScene.sceneObjects.Add(ent);
	//SceneManager::currentScene.sceneObjects.Add(ent2);
	//SceneManager::currentScene.sceneObjects.Add(ent3);
	SceneManager::currentScene.sceneObjects.Add(ent5);

	renderableMesh.shader = ShaderManager::GetMainShader();
	renderableMesh2.shader = ShaderManager::GetMainShader();
	renderableMesh3.shader = ShaderManager::GetMainShader();


	//Directional
	vec3 dirColor(1.0f, 1.0f, 1.0f);
	vec3 dir(0.0f, -0.5f, 1.0f);
	float dirInten = 1.1f;
	ECS::Entity* lightDir = world->create();
	lightDir->assign<TransformComponent>(Transform());
	lightDir->assign<LightComponent>(dirColor, dirInten, vec3(0), dir);

	//Point;
	vec3 color(1.0f, 1.0f, 1.0f);
	float inten = 1200;

	ECS::Entity* light1 = world->create();
	Transform transform1;
	transform1.position = vec3(-10.0f, 10.0f, 10.0f);
	light1->assign<TransformComponent>(transform1);
	light1->assign<LightComponent>(color, inten, vec3(0));

	ECS::Entity* light2 = world->create();
	Transform transform2;
	transform2.position = vec3(10.0f, 10.0f, 10.0f);
	light2->assign<TransformComponent>(transform2);
	light2->assign<LightComponent>(color, inten, vec3(0));


	ECS::Entity* light3 = world->create();
	Transform transform3;
	transform3.position = vec3(-10.0f, -10.0f, 10.0f);
	light3->assign<TransformComponent>(transform3);
	light3->assign<LightComponent>(color, inten, vec3(0));

	ECS::Entity* light4 = world->create();
	Transform transform4;
	transform4.position = vec3(10.0f, -10.0f, 10.0f);
	light4->assign<TransformComponent>(transform4);
	light4->assign<LightComponent>(color, inten, vec3(0));
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
		GUI_ShowMenuBar();
	windowHeight = height;
	windowWidth = width;
}

void Application::GUI_ShowMenuBar()
{
	// Menu Bar
	ImGui::BeginMenuBar();
	if (ImGui::BeginMenu("Menu"))
	{
		
		if (ImGui::MenuItem("New")) 
		{
			//Create and load new scene
		}
		// Buttons return true when clicked (most widgets return true when edited/activated)
		if (ImGui::MenuItem("Open", "Ctrl+O")) {}
		if (ImGui::MenuItem("Save Scene", "Ctrl+S")) 
		{  
			SceneManager::SaveScene("TestScene", *GetMainCamera());
		}
		if (ImGui::MenuItem("Load Scene", "Ctrl+L"))
		{

			SceneManager::LoadScene("TestScene.json", *GetMainCamera());
	
		}
		if (ImGui::MenuItem("Save As..")) {}
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
		ImGui::EndMenu();
	}
	ImGui::EndMenuBar();
}






void Application::ShowContentWindow()
{
	// ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
	// ImGui::BeginChild("Content Window", ImVec2(0, GetPercentOf(30, windowHeight)), true/*, window_flags*/);

	// ImGui::SetNextWindowContentSize(ImVec2(100 * MapModelPaths.size(), 0.0f));
	// ImGui::BeginChild("##ScrollingRegion", ImVec2(0, ImGui::GetFontSize() * 20), false, ImGuiWindowFlags_HorizontalScrollbar);
	// ImGui::Columns(MapModelPaths.size());
	// int ITEMS_COUNT = 1;
	// ImGuiListClipper clipper(ITEMS_COUNT);  // Also demonstrate using the clipper for large list
	// while (clipper.Step())
	// {
	// 	for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
	// 	{
	// 		int j = 0;
	// 		for (const auto& x : MapModelPaths)
	// 		{

	// 			ImGui::PushID(j);
	// 			int frame_padding = -1 + i;     // -1 = uses default padding
	// 			//ImGui::Image((void*)(intptr_t)TestTexId, ImVec2(400/5, 400/5));
	// 			if (ImGui::ImageButton((void*)(intptr_t)TestTexId, ImVec2(400 / 5, 400 / 5)))
	// 			{
	// 				loadEntityToScene(ecs, x.first);
	// 			}
	// 			ImGui::PopID();
	// 			ImGui::TextWrapped(x.first.c_str());
	// 			ImGui::NextColumn();

	// 			j++;
	// 		}
	// 	}
	// }
	// ImGui::Columns(1);
	// ImGui::EndChild();

	// ImGui::EndChild();
	// ImGui::PopStyleVar();
}


//bool showInspector = false;
//EntityHandle selectedEntity;

void Application::ShowSceneObjectList()
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

void Application::ShowInspector()
{
	// //Inspector
	// if (showInspector)
	// {
	// 	TransformComponent* TC = ecs.GetComponent<TransformComponent>(selectedEntity);
	// 	if (TC)
	// 	{
	// 		//position
	// 		float PosVec[3];
	// 		for (int i = 0; i < 3; i++)
	// 		{
	// 			PosVec[i] = TC->transform.position[i];
	// 		}
	// 		//Scale
	// 		float scale = TC->transform.scale.x;



	// 		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 1.0f);
	// 		ImGui::BeginChild("Inspector", ImVec2(GetPercentOf(20, windowWidth) - 45, GetPercentOf(70, windowHeight)), true/*, window_flags*/);

	// 		ImGui::DragFloat3("Position", PosVec, 0.01f);
	// 		ImGui::DragFloat("Scale", &scale, 0.01f, 0.001f, 1000.0f);

	// 		//position
	// 		for (int i = 0; i < 3; i++)
	// 			TC->transform.position[i] = PosVec[i];
	// 		//scale
	// 		for (int i = 0; i < 3; i++)
	// 			TC->transform.scale[i] = scale;


	// 		if (ImGui::Button("Remove Object"))
	// 		{
	// 			//cSceneManager::RemoveObjectFromScene(selectedEntity, ecs);
	// 			showInspector = false;
	// 		}
	// 		if (ImGui::Button("Close"))
	// 		{
	// 			showInspector = false;
	// 		}
	// 		ImGui::EndChild();
	// 		ImGui::PopStyleVar();
	// 	}


	// }
}

void Application::ShowEditor()
{
	// ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 1.0f);
	// ImGui::BeginChild("ViewPort", ImVec2(GetPercentOf(60, windowWidth), GetPercentOf(70, windowHeight)), true/*, window_flags*/);

	// ImGui::Image((void *)EditorTex, ImVec2(GetPercentOf(60, windowWidth), GetPercentOf(70, windowHeight)), ImVec2(0, 1), ImVec2(1, 0));
	// ImGui::EndChild();
	// ImGui::PopStyleVar();
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


bool Application::loadTextFileWithIncludes(NString& output, const NString& fileName,
		const NString& includeKeyword)
{
	NString absolutePath = Nx::FileSystem::GetPath(fileName);
	std::ifstream file;
	file.open(absolutePath.c_str());

	NString filePath = StringFuncs::getFilePath(fileName);
	std::stringstream ss;
	NString line;

	if(file.is_open()) {
		while(file.good()) {
			getline(file, line);
			
			if(line.find(includeKeyword) == NString::npos) {
				ss << line << "\n";
			} else {
				NString includeFileName = StringFuncs::split(line, ' ')[1];
				includeFileName =
					includeFileName.substr(1,includeFileName.length() - 2);

				NString toAppend;
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

