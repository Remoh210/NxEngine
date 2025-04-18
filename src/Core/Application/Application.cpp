
#include "Application.h"
#include <Core/Camera/Camera.h>
#include <Common/CommonTypes.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <Core/Components/TransformComponent/TransformComponent.h>
#include <Core/Components/LightComponent/LightComponent.h>
#include <Core/Components/SkinnedMeshComponent/SkinnedMeshComponent.h>
#include <Core/Components/Physics/RigidBodyComponent.h>
#include <Core/Components/Character/CharacterComponent.h>
#include <Core/Systems/RenderSystem.h>
#include <Core/Systems/Animator/AnimatorSystem.h>
#include <Core/Systems/Character/CharacterSystem.h>
#include <Core/FileSystem/FileSystem.h>
#include <Core/Graphics/DebugRenderer/DebugRenderer.h>
#include <Core/Graphics/Cubemap/CubemapManager.h>
#include <Core/Graphics/ShaderManager/ShaderManager.h>
#include <Core/Graphics/Animation/AnimationInfo.h>
#include <Core/Application/SceneManager/SceneManager.h>
#include <Core/Application/AssetManager/AssetManager.h>
#include <Core/Input/InputManager.h>
#include <Core/Application/Settings/GlobalSettings.h>
#include <Core/Systems/Physics/PhysicsSystem.h>
#include <Core/Time/NxTime.h>
#include <rendering/Sampler.h>
#include <rendering/RenderDevice.h>
#include <rendering/RenderTarget.h>
#include <rendering/AssetLoader.h>
#include <rendering/Shader.h>
#include <Editor/EditorRenderContext.h>
#include <Editor/EditorUI/EditorUI.h>

#include "Common/Math/Math.h"


//Standard includes
#include <fstream>
#include <sstream>

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
Window* Application::window = nullptr;
bool Application::bPlayingInEditor = false;

bool show_demo_window = false;

bool bInit = true;

//UI
uint32 fps = 0;
double currentFrame = glfwGetTime();
double lastFrame = 0;
double fpsTimeCounter = 0.0;
double msPerFrame = 0;
double msPerFrameToShow = 0;
uint32 fpsToShow = 0;


//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void resize_callback(GLFWwindow * window, int width, int height);
void key_callback(GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mods);

int MSAA_num = 8;
Application* Application::Create(float Width, float Height)
{
	return new Application(Width, Height);
}


//Test
float roughness = 0.1f;
float metallic = 0.9;

void Application::DrawDebugWindow()
{
	
	// if(bInit)
	// {
	// 	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
	// 	ImGui::SetNextWindowSize(ImVec2(100, 300));
	// }
	
	ImGuiWindowFlags window_flags;
	ImGui::Begin("DebugWindow");

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
		
	NString PIE_ButtonText = bPlayingInEditor ? "Stop PIE" : "Play In Editor";
	if(ImGui::Button(PIE_ButtonText.c_str()))
	{
		TogglePIE();
	}
		
	if (ImGui::Button("Draw Grid"))
	{
		editorRenderContext->ToggleGrid();
	}
		
	if (ImGui::Button("Draw Collision"))
	{
		auto physicsSystemNx = PhysicsSystem::Get();
		physicsSystemNx->ToggleDebugDraw();
	}
		
	if (ImGui::Button("PostFX"))
	{
		editorRenderContext->TogglePostFX();
	}
		
	if (ImGui::Button("Show demo"))
	{
		show_demo_window = true;
	}
		
		
	ImGui::Text("Environments:");
	if (ImGui::Button("Sky"))
	{
		editorRenderContext->GeneratePBRMapsFromTexture("res/textures/HDR/sky.jpg");
	}
	if (ImGui::Button("Road"))
	{
		editorRenderContext->GeneratePBRMapsFromTexture("res/textures/HDR/road.hdr");
	}
	if (ImGui::Button("Room"))
	{
		editorRenderContext->GeneratePBRMapsFromTexture("res/textures/HDR/newport_loft.hdr");
	}

	ImGui::End();

	//// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	if(bInit)
	{
		bInit = false;
	}
}

int Application::Run()
{
	Initialize();
	EditorUI::Initialize(renderDevice->GetShaderVersion(), window);

	ImGui::CreateContext();

	
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    //TODO: Fix this
	AssetLoader::SetShouldFlipVTexture(true);


	ECS::EntitySystem* renderSystem = world->registerSystem(new ECS::RenderableMeshSystem(editorRenderContext));
	ECS::EntitySystem* animatorSystem = world->registerSystem(new ECS::AnimatorSystem());
	ECS::EntitySystem* characterSystem = world->registerSystem(new ECS::CharacterSystem(MainCamera));

	PhysicsSystem::Initialize(world);
	AssetManager::SetPhysicsSystem(PhysicsSystem::Get());
	InputManager::Initialize(window);
	DebugRenderer::SetContext(editorRenderContext);
	DebugRenderer::SetShader(renderDevice);

	
	//debugRenderer.DrawDebugLine(vec3(0.f), vec3(0.0f, 30.0f, 0.0f), 5, vec3(0, 1, 0));
	
	//CreateDefaultScene();

	LoadDefaultScene();
	
	
	while (!window->ShouldClose())
	{
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		
		// render
		// ------
		//editorRenderContext->Clear(glm::vec4(0.34, 0.3, 0.5, 0), true);
		DebugRenderer::Update(deltaTime);
		world->tick(deltaTime);
		editorRenderContext->Flush();
		
		NxTime::Update(deltaTime);
		
		EditorUI::DrawEditorView(editorRenderContext);
		DrawDebugWindow();
		EditorUI::Draw();
		
		
		window->UpdateInput();
		window->Present();
		processInput(glfwGetCurrentContext());
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		EditorUI::UpdateWindows();
		glfwMakeContextCurrent(backup_current_context);
	}

	//Delete
	ShutDown();

	// UI Cleanup
	EditorUI::Shutdown();

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
		MainCamera = new Camera(glm::vec3(0.0f, 15.0f, 43.0f));
		MainCamera->bControlled = false;
		MainCamera->MovementSpeed = 50.0f;
	}
		
	deltaTime = 0;
}


void Application::Initialize()
{
	GlobalSettings::LoadSettings("Settings.json");

	int verMaj = GlobalSettings::GetAPIVersionMajor();
	int verMin = GlobalSettings::GetAPIVersionMinor();
	int sw = GlobalSettings::GetWindowWidth();
	int sh = GlobalSettings::GetWindowHeight();
	char title[50];
	sprintf(title, "OpenGL %d.%d [%dx%d]", verMaj, verMin, sw, sh);
	window = new Window(GlobalSettings::GetWindowWidth(), GlobalSettings::GetWindowHeight(), title);
	//TODO: Put to init UI
	window->SetMouseCallback
	(
		[this](int xpos, int ypos)
	{
		Application::GetMainCamera()->ProcessMouseMovement(xpos, ypos);
	}
	);
	window->SetMouseScrollCallback
	(
		[this](float yoffset)
	{
		if (!Application::GetIsPIE())
		{
			Application::GetMainCamera()->ProcessMouseScroll(yoffset);
		}
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

	NString ImpostorShaderText;
	loadTextFileWithIncludes(ImpostorShaderText, "res/shaders/ImpostorShader.glsl", "#include");
	Shader* ImpostorShader = new Shader(renderDevice, ImpostorShaderText);
	ShaderManager::AddPBRShader("IMPOSTOR_SHADER", ImpostorShader);


	NString SkinnedshaderText;
	loadTextFileWithIncludes(SkinnedshaderText, "res/shaders/PBR/main_skinned.glsl", "#include");
	Shader* SkinnedShader = new Shader(renderDevice, SkinnedshaderText);;
	ShaderManager::AddPBRShader("MAIN_SKELETAL", SkinnedShader);

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

	NString screenShaderText;
	loadTextFileWithIncludes(screenShaderText, "res/shaders/ScreenShader.glsl", "#include");
	Shader* screenShader = new Shader(renderDevice, screenShaderText);
	ShaderManager::AddPBRShader("SCREEN_SHADER", screenShader);

	NString chromaShaderText;
	loadTextFileWithIncludes(chromaShaderText, "res/shaders/PostFX/Chroma.glsl", "#include");
	Shader* chromaShader = new Shader(renderDevice, chromaShaderText);
	ShaderManager::AddPostFXshader("CHROMA_SHADER", chromaShader);

	//Init Shaders**********************************************************

	editorSampler = new Sampler(renderDevice, SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR, FILTER_LINEAR_MIPMAP_LINEAR);


	DrawParams drawParams;
	drawParams.primitiveType = PRIMITIVE_TRIANGLES ;
	drawParams.faceCulling = FACE_CULL_NONE;
	drawParams.shouldWriteDepth = true;
	drawParams.depthFunc = DRAW_FUNC_LESS;

	int ha = window->GetHeight();
	int wa = window->GetWidth();
	mat4 projection = glm::perspective(glm::radians(45.0f), (float)window->GetWidth() / (float)window->GetHeight(), 0.1f, 10000.0f);

	CubemapManager::Inititialize(renderDevice, PBREqToCubeshader, IrConvShader);
	editorRenderTarget = new RenderTarget(renderDevice);
	editorRenderContext = new EditorRenderContext(renderDevice, editorRenderTarget, drawParams, editorSampler, projection, MainCamera);



	window->SetFrameBufferResizeCallback
	(
		[this](float x, float y)
	{
		editorRenderContext->ResizeViewPort(x, y);
		EditorUI::OnWindowResized(x, y);
	}
	);

	//ECS
	world = ECS::World::createWorld();
	
	SceneManager::SetECS(world);
	SceneManager::SetCamera(GetMainCamera());
	SceneManager::SetRenderDevice(renderDevice);
	ShaderManager::SetRenderDevice(renderDevice);
	AssetManager::SetRenderDevice(renderDevice);
}

void Application::ShutDown()
{
	world->destroyWorld();
	delete renderDevice;
	delete editorSampler;
	delete editorRenderTarget;
	delete editorRenderContext;
}

void Application::CreateDefaultScene()
{
	NString TEST_MODEL_PISTOL = "res/models/pistol_test.glb";
	NString TEST_MODEL_DUST_MAP = "res/models/dust/fbx/dust.fbx";
	NString TEST_MODEL_FILE_PBR_TEST = "res/models/PBRTest.glb";

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
	
#pragma region loadMeshes
	NxArray<IndexedModel> PBRLoadedMeshes;
	NxArray<uint32> PBRMaterialIndices;
	NxArray<MaterialSpec> PBRLoadedMaterials;
	AssetLoader::LoadModel(TEST_MODEL_PISTOL, PBRLoadedMeshes, PBRMaterialIndices, PBRLoadedMaterials);

	ECS::Entity* pistolEntity = world->create();
	StaticMeshComponent pistolMeshComp;
	
	for(int i = 0; i < PBRLoadedMeshes.size(); i++)
	{
		MeshInfo* curMesh = new MeshInfo();
		curMesh->vertexArray = new VertexArray(renderDevice, PBRLoadedMeshes[i], USAGE_STATIC_DRAW);
		pistolMeshComp.meshes.push_back(curMesh);

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

	pistolMeshComp.shader = ShaderManager::GetMainShader();
	pistolMeshComp.meshAssetFile = TEST_MODEL_PISTOL;
	
	TransformComponent pistolTransformComp;
	pistolTransformComp.transform.position = vec3(-30.1f, 20.0f, -40.0f);
	pistolTransformComp.transform.rotation = vec3(0 , 0.0f, 0.f);
	pistolTransformComp.rotSpeed = 15.0f;
	pistolTransformComp.transform.scale = vec3(0.9);
	
	SceneManager::currentScene.MakeNewObject("Pistol", pistolEntity);

	//load map
	ECS::Entity* dustMapEntity = world->create();
	StaticMeshComponent dustMapMeshComp;
	dustMapMeshComp.bIsVisible = true;
	nPhysics::iShape* CurShape6;
	AssetManager::ImportModelGenerateCollider(TEST_MODEL_DUST_MAP, dustMapMeshComp.meshes, nPhysics::SHAPE_TYPE_MESH, CurShape6);
	dustMapMeshComp.meshAssetFile = TEST_MODEL_DUST_MAP;
	dustMapMeshComp.shader = ShaderManager::GetMainShader();
	dustMapMeshComp.numInst = 1;
	TransformComponent dustTransformComp;
	dustTransformComp.transform.position = vec3(-500.0f, 5.0f, -30.0f);
	dustTransformComp.transform.rotation = vec3(-90, 0, 0);


	RigidBodyComponent dustMapRigidBodyComp(dustMapEntity);
	dustMapRigidBodyComp.bIsStatic = true;
	nPhysics::sRigidBodyDef dustMapRigidBodyDef;
	dustMapRigidBodyDef.Orientation = vec3(
		glm::radians(dustTransformComp.transform.rotation[0]),
		glm::radians(dustTransformComp.transform.rotation[1]),
		glm::radians(dustTransformComp.transform.rotation[2]));
	
	dustMapRigidBodyDef.Position = dustTransformComp.transform.position.ToVec();
	dustMapRigidBodyDef.Mass = 0.0f;
	dustMapRigidBodyDef.GameObjectName = "dust";
	dustMapRigidBodyDef.Scale = dustTransformComp.transform.scale.ToVec();

	dustMapRigidBodyComp.rigidBody = PhysicsSystem::Get()->GetFactory()->CreateRigidBody(dustMapRigidBodyDef, CurShape6);
	PhysicsSystem::Get()->GetWorld()->AddBody(dustMapRigidBodyComp.rigidBody);

	SceneManager::currentScene.MakeNewObject("Dust Map", dustMapEntity);
	
#pragma endregion

#pragma region Load Lights
	//Directional
	vec3 dirColor(1.0f, 1.0f, 1.0f);
	vec3 dir(0.0f, -0.5f, 1.0f);
	float dirInten = 1.1f;
	ECS::Entity* lightDirEntity = world->create();
	LightComponent dirLightComp(lightDirEntity);
	dirLightComp.color = dirColor;
	dirLightComp.direction = dir;
	dirLightComp.intensity = dirInten;
	
	SceneManager::currentScene.MakeNewObject("Dir light 1", lightDirEntity);
	
#pragma endregion 
	
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
	NString TEST_MODEL_FILE6 = "res/models/dust/fbx/dust.fbx";
	//NString TEST_MODEL_FILE6 = "res/models/terrain.ply";
	NString TEST_MODEL_FILE7 = "res/models/PBRTest.glb";
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

#pragma region loadMeshes
	//model 1


	NxArray<IndexedModel> models;
	NxArray<uint32> modelMaterialIndices;
	NxArray<MaterialSpec> modelMaterials;
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
	renderableMesh.meshes.push_back(meshInfo1);
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
	renderableMesh2.meshes.push_back(meshInfo2);
	renderableMesh2.numInst = 100;
	TransformComponent transformComp2;
	transformComp2.transform.scale = vec3(7.0f);




	NxArray<IndexedModel> PBRLoadedMeshes;
	NxArray<uint32> PBRMaterialIndices;
	NxArray<MaterialSpec> PBRLoadedMaterials;
	AssetLoader::LoadModel(TEST_MODEL_FILE3, PBRLoadedMeshes, PBRMaterialIndices, PBRLoadedMaterials);

	StaticMeshComponent renderableMesh3;
	
	for(int i = 0; i < PBRLoadedMeshes.size(); i++)
	{
		MeshInfo* curMesh = new MeshInfo();
		curMesh->vertexArray = new VertexArray(renderDevice, PBRLoadedMeshes[i], USAGE_STATIC_DRAW);
		renderableMesh3.meshes.push_back(curMesh);

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
	renderableMesh3.meshAssetFile = TEST_MODEL_FILE3;
	renderableMesh3.bIsImported = true;
	TransformComponent transformComp3;
	transformComp3.transform.position = vec3(-30.1f, 20.0f, -40.0f);
	transformComp3.transform.rotation = vec3(0 , 0.0f, 0.f);
	transformComp3.rotSpeed = 15.0f;
	transformComp3.transform.scale = vec3(1.0);
	RigidBodyComponent rbPistol;
	NxArray<MeshInfo*> meshsesToAdd;
	
	nPhysics::iShape* pistolShape;
    AssetManager::ImportModelGenerateCollider(TEST_MODEL_FILE3, meshsesToAdd, nPhysics::SHAPE_TYPE_BOX, pistolShape);

	nPhysics::sRigidBodyDef PistolRbdef;
	PistolRbdef.Mass = 100.0f;
	PistolRbdef.Position = transformComp3.transform.position.ToVec();
	PistolRbdef.Scale = transformComp3.transform.scale.ToVec();
	//PistolRbdef.AngularVelocity = vec3(10.1f, 12.5f, -11.1f);
	PistolRbdef.quatOrientation = quat(transformComp3.transform.rotation.ToVec());
	nPhysics::iRigidBody* PistolRB = PhysicsSystem::Get()->GetFactory()->CreateRigidBody(PistolRbdef, pistolShape);
	//PistolRB->SetEulerRotation(transformComp3.transform.rotation.ToVec());
	PhysicsSystem::Get()->GetWorld()->AddBody(PistolRB);


	MeshInfo* pbrTestMesh = new MeshInfo();
	pbrTestMesh->vertexArray = new VertexArray(renderDevice, PrimitiveGenerator::CreateSphere(1.0f, 36, 36, vec3(0.0f)), BufferUsage::USAGE_DYNAMIC_DRAW);
	Material* material4 = new Material();

	material4->textures[TEXTURE_ALBEDO] = albedo;
	material4->textures[TEXTURE_NORMAL] = normal;
	material4->textures[TEXTURE_METALLIC] = metallic;
	material4->textures[TEXTURE_ROUGHNESS] = roughness;
	material4->textures[TEXTURE_AO] = ao;

	//material3.diffuseTextures.push_back(&testtex);
	pbrTestMesh->material = material4;
	StaticMeshComponent renderableMesh4;
	renderableMesh4.meshAssetFile = "Mesh4";
	renderableMesh4.shader = ShaderManager::GetMainShader();
	renderableMesh4.meshes.push_back(pbrTestMesh);
	TransformComponent transformComp4;
	transformComp4.transform.position = vec3(20, 20, -40);
	transformComp4.transform.scale = vec3(5);
	transformComp4.rotSpeed = 20.2f;



	MeshInfo* mesh5 = new MeshInfo();
	IndexedModel sphereIndexedModel = PrimitiveGenerator::CreateSphere(1.0f, 12, 12, vec3(0.0f));
	mesh5->vertexArray = new VertexArray(renderDevice, sphereIndexedModel, BufferUsage::USAGE_DYNAMIC_DRAW);
	Material* material5 = new Material();
	//material5->color(1.0);
	material5->metallic = 0.99;
	material5->roughness = 0.01;
	material5->color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	mesh5->material = material5;
	StaticMeshComponent renderableMesh5;
	renderableMesh5.meshAssetFile = "Mesh5";
	renderableMesh5.shader = ShaderManager::GetMainShader();
	renderableMesh5.meshes.push_back(mesh5);
	TransformComponent transformComp5;
	transformComp5.transform.position = vec3(0, 20, -40);
	//transformComp.transform.rotation = vec3(5.9f, -0.15f, -50.0f);
	transformComp5.transform.scale = vec3(5);
	RigidBodyComponent rb5;
	nPhysics::iShape* CurShape5;
	nPhysics::sRigidBodyDef def5;
	def5.Position = transformComp5.transform.position.ToVec();
	def5.AngularVelocity = vec3(2, 0, 2);
	def5.Mass = 110.0f;
	def5.GameObjectName = "test5";
	//def5.Scale = vec3(6.1f);

	

	CurShape5 = PhysicsSystem::Get()->GetFactory()->CreateSphereShape(5.2);
	rb5.rigidBody = PhysicsSystem::Get()->GetFactory()->CreateRigidBody(def5, CurShape5);
	PhysicsSystem::Get()->GetWorld()->AddBody(rb5.rigidBody);


	StaticMeshComponent renderableMesh6;
	renderableMesh6.bIsImported = true;
	renderableMesh6.bIsVisible = true;
	nPhysics::iShape* CurShape6;
	AssetManager::ImportModelGenerateCollider(TEST_MODEL_FILE6, renderableMesh6.meshes, nPhysics::SHAPE_TYPE_MESH, CurShape6);
	renderableMesh6.meshAssetFile = TEST_MODEL_FILE6;
	renderableMesh6.shader = ShaderManager::GetMainShader();
	renderableMesh6.numInst = 1;
	TransformComponent transformComp6;
	transformComp6.transform.position = vec3(-500.0f, 5.0f, -30.0f);
	//transformComp6.transform.position = vec3(-180.1f, -20.0f, -40.0f);
	transformComp6.transform.rotation = vec3(-90, 0, 0);
	//transformComp6.transform.scale = vec3(0.9);
	RigidBodyComponent rb6;

	rb6.bIsStatic = true;
	nPhysics::sRigidBodyDef def6;
	def6.Orientation = vec3(glm::radians(transformComp6.transform.rotation[0]), glm::radians(transformComp6.transform.rotation[1]), glm::radians(transformComp6.transform.rotation[2]));
	def6.Position = transformComp6.transform.position.ToVec();
	def6.Mass = 0.0f;
	def6.GameObjectName = "test6";
	def6.Scale = transformComp6.transform.scale.ToVec();
	//CurShape6 = PhysicsSystem::Get()->GetFactory()->CreatePlaneShape(vec3(0, 1, 0), 0);
	//nPhysics::GL_Triangle* GLTriangle = new nPhysics::GL_Triangle[curModelInfo.pMeshData->numberOfTriangles];

	rb6.rigidBody = PhysicsSystem::Get()->GetFactory()->CreateRigidBody(def6, CurShape6);
	PhysicsSystem::Get()->GetWorld()->AddBody(rb6.rigidBody);



	StaticMeshComponent renderableMesh7;

	NxArray<MeshInfo*> meshsesToAdd7;
	nPhysics::iRigidBody* rbToAdd7 = nullptr;
	//nPhysics::iShape* CurShape6; 
	
	renderableMesh7.meshes = AssetManager::ImportModel(TEST_MODEL_FILE7);

	renderableMesh7.meshAssetFile = TEST_MODEL_FILE7;
	renderableMesh7.shader = ShaderManager::GetMainShader();
	renderableMesh7.numInst = 1;
	renderableMesh7.bIsImported = true;
	TransformComponent transformComp7;
	transformComp7.transform.position = vec3(-20.1f, 10.0f, -40.0f);
	transformComp7.transform.rotation = vec3(0.0f, 0.0f, 0.f);
	transformComp7.transform.scale = vec3(50.0);
	transformComp7.rotSpeed = 10.0f;
	//RigidBodyComponent rigidBComp;


	//rbToAdd->SetPosition(transformComp7.transform.position.ToVec());
	//rbToAdd->SetMass(100);
	//rbToAdd->SetEulerRotation(transformComp7.transform.rotation.ToVec());;

	//rigidBComp.rigidBody = rbToAdd;

#pragma endregion

#pragma region Skinned Mesh
	SkinnedMeshComponent skinnedMesh;
	skinnedMesh.skinnedMeshInfo = AssetManager::ImportModelSkeletal("res/models/chan.fbx");
	//skinnedMesh.skinnedMeshInfo->mesh->material->textures.clear();
	skinnedMesh.skinnedMeshInfo->mesh->drawParams.faceCulling = FACE_CULL_BACK;
	skinnedMesh.skinnedMeshInfo->mesh->drawParams.depthFunc = DRAW_FUNC_LESS;
	skinnedMesh.skinnedMeshInfo->mesh->drawParams.shouldWriteDepth = true;

	//skinnedMesh.meshAssetFile = TEST_MODEL_FILE7;
	//skinnedMesh.shader = ShaderManager::GetMainShader();
	//skinnedMesh.numInst = 1;
	TransformComponent transformCompSkinned;
	transformCompSkinned.transform.position = vec3(90.1f, 30.0f, -40.0f);
	transformCompSkinned.transform.rotation = vec3(0.0f, 0.0f, 0.f);
	transformCompSkinned.transform.scale = vec3(0.55);
	RigidBodyComponent charRbComp;
	charRbComp.offset = vec3f(0.0f, -48.0f, 0.0f);
	nPhysics::iShape* CurShape8;
	nPhysics::sRigidBodyDef def8;
	def8.Position = transformCompSkinned.transform.position.ToVec();
	def8.Mass = 1600.0f;
	def8.isPlayer = true;
	CurShape8 = PhysicsSystem::Get()->GetFactory()->CreateCapsuleShape(110.0f, 20.f, 1.0f);
	charRbComp.rigidBody = PhysicsSystem::Get()->GetFactory()->CreateRigidBody(def8, CurShape8);
	PhysicsSystem::Get()->GetWorld()->AddBody(charRbComp.rigidBody);
	AnimatorComponent animComp;
	animComp.animations["idle"] = AssetManager::ImportAnimation("res/models/animations/sad_idle_anim.fbx", "idle");
	animComp.animations["shoot"] = AssetManager::ImportAnimation("res/models/animations/shoot_anim.fbx", "shoot");
	animComp.animations["walk_left"] = AssetManager::ImportAnimation("res/models/animations/walk_left.fbx", "walk_left");
	animComp.animations["walk_right"] = AssetManager::ImportAnimation("res/models/animations/walk_right.fbx", "walk_right");
	animComp.animations["walk_forward"] = AssetManager::ImportAnimation("res/models/animations/walk_forward.fbx", "walk_forward");
	AnimationState idleState;
	idleState.transitionMap["walk_forward"] = AnimationState::AnimTransition(InputKey::KEY_W);
	idleState.transitionMap["walk_left"] = AnimationState::AnimTransition(InputKey::KEY_A);
	idleState.transitionMap["walk_right"] = AnimationState::AnimTransition(InputKey::KEY_D);
	idleState.transitionMap["shoot"] = AnimationState::AnimTransition(InputKey::KEY_SPACE);
	idleState.activeAnimation.name = "idle";
	idleState.activeAnimation.bHasExitTime = true;
	animComp.animationStates["idle"] = idleState;
	//idleState.nextAnimation.name = "shoot";
	//idleState.transitionKey = InputKey::KEY_SPACE;
	CharacterComponent characterComponent;
	characterComponent.movementSpeed = 10.0f;


	animComp.currentState = idleState;
	animComp.InitialState = idleState;

	AnimationState ShotState; 
	ShotState.activeAnimation.bHasExitTime = true;
	ShotState.activeAnimation.name = "shoot";
	animComp.animationStates["shoot"] = ShotState;

	AnimationState WalkLeftState;
	WalkLeftState.activeAnimation.name = "walk_left";
	animComp.animationStates["walk_left"] = WalkLeftState;

	AnimationState WalkRightState;
	WalkRightState.activeAnimation.name = "walk_right";
	animComp.animationStates["walk_right"] = WalkRightState;

	AnimationState WalkForwardState;
	WalkForwardState.activeAnimation.name = "walk_forward";
	animComp.animationStates["walk_forward"] = WalkForwardState;

#pragma endregion
	
	ECS::Entity* ent3 = world->create();
	ent3->assign<TransformComponent>(transformComp3);
	ent3->assign<StaticMeshComponent>(renderableMesh3);
	ent3->assign<RigidBodyComponent>(ent3, PistolRB);


	ECS::Entity* ent4 = world->create();
	ent4->assign<TransformComponent>(transformComp4);
	ent4->assign<StaticMeshComponent>(renderableMesh4);


	ECS::Entity* ent5 = world->create();
	ent5->assign<TransformComponent>(transformComp5);
	ent5->assign<StaticMeshComponent>(renderableMesh5);
	ent5->assign<RigidBodyComponent>(rb5);

	// ECS::Entity* ent6 = world->create();
	// ent6->assign<TransformComponent>(transformComp6);
	// ent6->assign<StaticMeshComponent>(renderableMesh6);
	// ent6->assign<RigidBodyComponent>(rb6);

	ECS::Entity* ent7 = world->create();
	ent7->assign<TransformComponent>(transformComp7);
	ent7->assign<StaticMeshComponent>(renderableMesh7);
	//ent7->assign<RigidBodyComponent>(rigidBComp);

	ECS::Entity* ent8 = world->create();
	ent8->assign<TransformComponent>(transformCompSkinned);
	ent8->assign<SkinnedMeshComponent>(skinnedMesh);
	ent8->assign<AnimatorComponent>(animComp);
	ent8->assign<CharacterComponent>(characterComponent);
	ent8->assign<RigidBodyComponent>(charRbComp);

	//SceneManager::currentScene.sceneObjects.push_back(ent);
	//SceneManager::currentScene.sceneObjects.push_back(ent2);
	//SceneManager::currentScene.sceneObjects.push_back(ent3);
	SceneManager::currentScene.MakeNewObject("Red sphere", ent5);
	SceneManager::currentScene.MakeNewObject("Rust", ent4);
	SceneManager::currentScene.MakeNewObject("Pistol", ent3);
	//SceneManager::currentScene.MakeNewObject("Dust Map", ent6);
	SceneManager::currentScene.MakeNewObject("Bottle", ent7);
	SceneManager::currentScene.MakeNewObject("SkinnedMesh", ent8);

	renderableMesh.shader = ShaderManager::GetMainShader();
	renderableMesh2.shader = ShaderManager::GetMainShader();
	renderableMesh3.shader = ShaderManager::GetMainShader();


	//********************** lights****************************************************

	MeshInfo* PointLightImposter = new MeshInfo();
	PointLightImposter->material = new Material();
	PointLightImposter->material->textures[TEXTURE_ALBEDO] = AssetManager::ImportTexture(renderDevice, "res/textures/default/icons/point_light.png", PixelFormat::FORMAT_RGBA);
	PointLightImposter->vertexArray = new VertexArray(renderDevice, PrimitiveGenerator::CreateQuad(), BufferUsage::USAGE_STATIC_DRAW);

	//Directional
	vec3 dirColor(1.0f, 1.0f, 1.0f);
	vec3 dir(0.0f, -0.5f, 1.0f);
	float dirInten = 1.1f;
	ECS::Entity* lightDir = world->create();
	lightDir->assign<TransformComponent>(lightDir, Transform());
	lightDir->assign<LightComponent>(lightDir, dirColor, dirInten, vec3(0), dir);
	SceneManager::currentScene.MakeNewObject("Dir light 1", lightDir);

	//Directional
	vec3 dirColor2(1.0f, 1.0f, 1.0f);
	vec3 dir2(0.0f, -0.1f, -0.06f);
	float dirInten2 = 10.1f;
	ECS::Entity* lightDir2 = world->create();
	MeshInfo* lightDir2Mesh = new MeshInfo();
	lightDir2Mesh->material = new Material();
	lightDir2Mesh->material->textures[TEXTURE_ALBEDO] = AssetManager::ImportTexture(renderDevice, "res/textures/default/icons/dir_light.png", PixelFormat::FORMAT_RGBA);
	lightDir2Mesh->vertexArray = new VertexArray(renderDevice, PrimitiveGenerator::CreateQuad(), BufferUsage::USAGE_STATIC_DRAW);
	ImpostorComponent lightDir2Impostor;
	lightDir2Impostor.mesh = lightDir2Mesh;
	lightDir2->assign<TransformComponent>(lightDir2, Transform(vec3(), vec3(), vec3(2.0f, 2.0f, 0.0f)));
	lightDir2->assign<LightComponent>(lightDir2, dirColor2, dirInten2, vec3(0), dir2);
	lightDir2->assign<ImpostorComponent>(lightDir2Impostor);
	SceneManager::currentScene.MakeNewObject("Dir light 2", lightDir2);

	//Point;
	vec3 color(1.0f, 1.0f, 1.0f);
	float inten = 1200;

	ECS::Entity* light1 = world->create();
	Transform transform1;
	transform1.position = vec3(-10.0f, 10.0f, 10.0f);
	ImpostorComponent light1ImpostorComp;
	light1ImpostorComp.mesh = PointLightImposter;
	light1->assign<TransformComponent>(light1, transform1);
	light1->assign<LightComponent>(light1, color, inten, vec3(0));
	light1->assign<ImpostorComponent>(light1ImpostorComp);
	
	SceneManager::currentScene.MakeNewObject("point light 1", light1);

	ECS::Entity* light2 = world->create();
	Transform transform2;
	transform2.position = vec3(10.0f, 10.0f, 10.0f);
	ImpostorComponent light2ImpostorComp;
	light2ImpostorComp.mesh = PointLightImposter;
	light2->assign<TransformComponent>(light2, transform2);
	light2->assign<LightComponent>(light2, color, inten, vec3(0));
	light2->assign<ImpostorComponent>(light2ImpostorComp);
	SceneManager::currentScene.MakeNewObject("point light 2", light2);


	ECS::Entity* light3 = world->create();
	Transform transform3;
	transform3.position = vec3(-10.0f, -10.0f, 10.0f);
	ImpostorComponent light3ImpostorComp;
	light3ImpostorComp.mesh = PointLightImposter;
	light3->assign<TransformComponent>(light3, transform3);
	light3->assign<LightComponent>(light3, color, inten, vec3(0));
	light3->assign<ImpostorComponent>(light3ImpostorComp);
	SceneManager::currentScene.MakeNewObject("point light 3", light3);

	ECS::Entity* light4 = world->create();
	Transform transform4;
	transform4.position = vec3(10.0f, -10.0f, 10.0f);
	ImpostorComponent light4ImpostorComp;
	light4ImpostorComp.mesh = PointLightImposter;
	light4->assign<TransformComponent>(light4, transform4);
	light4->assign<LightComponent>(light4, color, inten, vec3(0));
	light4->assign<ImpostorComponent>(light4ImpostorComp);
	SceneManager::currentScene.MakeNewObject("point light 4", light4);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void Application::processInput(GLFWwindow *window)
{
	//if (GetIsPIE()) { return; }
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
	//GUI_ShowMenuBar();
	windowHeight = height;
	windowWidth = width;
}

void Application::TogglePIE()
{
	if (bPlayingInEditor)
	{
		bPlayingInEditor = false;
		glfwSetInputMode(window->GetWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else
	{
		bPlayingInEditor = true;
		glfwSetInputMode(window->GetWindowHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}

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
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		if (Application::GetIsPIE()) { return; }

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