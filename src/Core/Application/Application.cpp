//
//#include "Application.h"
//#include <imgui_impl_glfw.h>
//#include <imgui_impl_opengl3.h>
////#include "../Rendering/LOpenGL/model.h"
////#include "../Rendering/RenderComponent.h"
////#include "../Rendering/RenderSystem.h"
//#include "SceneManager/cSceneManager.h"
//#include <filesystem>
//
//uint32 Application::numInstances = 0;
//
//GLint TestTexId;
//
//float lastX = 1200 / 2.0f;
//float lastY = 700 / 2.0f;
//bool firstMouse = true;
//
//double Application::gScrollOffset = 0;
//double Application::gXoffset = 0;
//double Application::gYoffset = 0;
//Camera* Application::MainCamera = nullptr;
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void resize_callback(GLFWwindow * window, int width, int height);
//void key_callback(GLFWwindow* window,
//	int key,
//	int scancode,
//	int action,
//	int mods);
//
//
//
//unsigned int gBuffer;
//unsigned int gPosition, gNormal, gAlbedoSpec;
//
//// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
//GLuint FramebufferName = 0;
//GLuint renderedTexture;
//GLuint depthrenderbuffer;
//
//const unsigned int NR_LIGHTS = 32;
//std::vector<glm::vec3> lightPositions;
//std::vector<glm::vec3> lightColors;
//Map<string, Shader> MainPipeline;
//
//unsigned int quadVAO = 0;
//unsigned int quadVBO;
//void renderQuad()
//{
//	if (quadVAO == 0)
//	{
//		float quadVertices[] = {
//			// positions        // texture Coords
//			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
//			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
//			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
//			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
//		};
//		// setup plane VAO
//		glGenVertexArrays(1, &quadVAO);
//		glGenBuffers(1, &quadVBO);
//		glBindVertexArray(quadVAO);
//		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
//		glEnableVertexAttribArray(0);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//		glEnableVertexAttribArray(1);
//		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//	}
//	glBindVertexArray(quadVAO);
//	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//	glBindVertexArray(0);
//}
//
//
//Application* Application::Create(float Width, float Height)
//{
//	return new Application(Width, Height);
//}
//
//int Application::Run()
//{
//	ECS ecs;
//	RenderSystem renderSystem(GetMainCamera(), MainPipeline);
//	SystemList systemList;
//	systemList.AddSystem(renderSystem);
//
//	cSceneManager::LoadScene("test", ecs, *GetMainCamera());
//
//	//Name, Path
//	
//	for (std::filesystem::recursive_directory_iterator i("../assets/models"), end; i != end; ++i)
//	{
//		if (!is_directory(i->path()))
//		{
//			if (i->path().extension().string() == ".obj")
//			{
//				string ModelPath = i->path().parent_path().filename().string()+ "/" + i->path().filename().string();
//				//ModelFiles.push_back(i->path().filename().string());
//				MapModelPaths.insert(std::pair<string, string>(i->path().filename().string(), ModelPath));
//				DEBUG_LOG_TEMP(ModelPath.c_str());
//			}
//		}
//		
//	}
//
//
//	TestTexId = Model::TextureFromFile("default_model.jpg", "../assets/engine_default/textures");
//
//	uint32 fps = 0;
//	double currentFrame = glfwGetTime();
//	double lastFrame = 0;
//	double fpsTimeCounter = 0.0;
//	double msPerFrame = 0;
//	double msPerFrameToShow = 0;
//	uint32 fpsToShow = 0;
//
//	ImGui::CreateContext();
//	ImGui_ImplGlfw_InitForOpenGL(GetWindow(), true);
//	ImGui_ImplOpenGL3_Init("#version 130");
//	ImGui::StyleColorsDark();
//	bool show_demo_window = true;
//	bool show_another_window = false;
//	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
//	bool p_open = true;
//
//	while (true)
//	{
//
//		if (window && glfwWindowShouldClose(window))
//		{
//			glfwDestroyWindow(window);
//			glfwTerminate();
//			exit(EXIT_SUCCESS);
//			return 0;
//		}
//
//
//		// Start the Dear ImGui frame
//		ImGui_ImplOpenGL3_NewFrame();
//		ImGui_ImplGlfw_NewFrame();
//
//
//
//		ImGui::NewFrame();
//
//		ImVec2 VecScreen(windowWidth, windowHeight);
//		ImVec2 VecPos(0, 0);
//		ImGui::SetNextWindowSize(VecScreen);
//		ImGui::SetNextWindowPos(VecPos);
//
//
//
//		ImGuiWindowFlags window_flags = 0;
//		//window_flags |= ImGuiWindowFlags_NoTitleBar;
//		//if (no_scrollbar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
//		window_flags |= ImGuiWindowFlags_MenuBar;
//		window_flags |= ImGuiWindowFlags_NoMove;
//		window_flags |= ImGuiWindowFlags_NoResize;
//		//window_flags |= ImGuiWindowFlags_NoCollapse;
//		//if (no_nav)             window_flags |= ImGuiWindowFlags_NoNav;
//		//if (no_background)      window_flags |= ImGuiWindowFlags_NoBackground;
//		//if (no_bring_to_front)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
//		//if (no_close)           p_open = NULL; // Don't pass our bool* to Begin
//
//		
//		ImGui::Begin("Main Window", &p_open, window_flags);                          // Create a window called "Hello, world!" and append into it.
//		GUI_ShowMenuBar(ecs);
//
//		currentFrame = glfwGetTime();
//		deltaTime = currentFrame - lastFrame;
//		lastFrame = currentFrame;
//		fpsTimeCounter += deltaTime;
//		fps++;
//		ImGui::Text("%f ms (%d fps)", msPerFrameToShow, fpsToShow);
//		if (fpsTimeCounter >= 1.0) {
//			fpsToShow = fps;
//			double msPerFrame = 1000.0 / (double)fps;
//			msPerFrameToShow = msPerFrame;
//			fpsTimeCounter = 0;
//			fps = 0;
//		}
//
//
//		// Scene Tree
//		{
//			ShowSceneObjectList(ecs);
//		}
//		ImGui::SameLine();
//		//Editor viewport
//		{
//			ShowEditor(renderedTexture);
//
//		}
//		ImGui::SameLine();
//		//Inspector
//		{
//			ShowInspector(ecs);
//		}
//		ImGui::NewLine();
//		//Content Window
//		{
//			ShowContentWindow(ecs);
//		}
//
//
//		//// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
//		if (show_demo_window)
//		ImGui::ShowDemoWindow(&show_demo_window);
//
//		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
//		{
//			static float f = 0.0f;
//			static int counter = 0;
//			ImGui::Begin("Settings");                          // Create a window called "Hello, world!" and append into it.
//
//			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
//			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
//
//			ImGui::SameLine();
//			ImGui::Text("counter = %d", counter);
//
//			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//			ImGui::End();
//		}
//
//
//
//
//
//
//
//
//
//		ImGui::End();
//
//
//		// 1. geometry pass: render scene's geometry/color data into gbuffer
//	// -----------------------------------------------------------------
//		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glm::mat4 projection = glm::perspective(glm::radians(GetMainCamera()->Zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
//		glm::mat4 view = GetMainCamera()->GetViewMatrix();
//		MainPipeline.find("geometry_pass")->second.use();
//		MainPipeline.find("geometry_pass")->second.setMat4("projection", projection);
//		MainPipeline.find("geometry_pass")->second.setMat4("view", view);
//
//
//		//DisplayEntyties(ecs);
//		ecs.UpdateSystems(systemList, deltaTime);
//
//
//
//		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
//		glViewport(0, 0, GetPercentOf(60, windowWidth), GetPercentOf(70, windowHeight)); // Render on the whole framebuffer, complete from the lower left corner to the upper right
//
//		//
//		// 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
//			// -----------------------------------------------------------------------------------------------------------------------
//		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		MainPipeline.find("lighting_pass")->second.use();
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, gPosition);
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, gNormal);
//		glActiveTexture(GL_TEXTURE2);
//		glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
//		// send light relevant uniforms
//		for (unsigned int i = 0; i < lightPositions.size(); i++)
//		{
//			MainPipeline.find("lighting_pass")->second.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
//			MainPipeline.find("lighting_pass")->second.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
//			// update attenuation parameters and calculate radius
//			const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
//			const float linear = 0.7;
//			const float quadratic = 1.8;
//			MainPipeline.find("lighting_pass")->second.setFloat("lights[" + std::to_string(i) + "].Linear", linear);
//			MainPipeline.find("lighting_pass")->second.setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
//			// then calculate radius of light volume/sphere
//			const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
//			float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
//			MainPipeline.find("lighting_pass")->second.setFloat("lights[" + std::to_string(i) + "].Radius", radius);
//		}
//		MainPipeline.find("lighting_pass")->second.setVec3("viewPos", GetMainCamera()->Position);
//
//
//
//
//
//		//// finally render quad
//		//renderQuad();
//
//		// 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
//		// ----------------------------------------------------------------------------------
//		//glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
//		//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FramebufferName); // write to default framebuffer
//		//// blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
//		//// the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the 		
//		//// depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
//		////glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, windowWidth, windowHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
//		//glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
//
//
//
//
//
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		glViewport(0, 0, windowWidth, windowHeight);
//		glClearColor(1.f, 1.f, 1.f, 1.f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		// Bind our texture in Texture Unit 0
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, renderedTexture);
//		ImGui::Render();
//		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//
//		processInput(window);
//		
//
//
//		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	return 0;
//}
//
//Application::~Application()
//{
//}
//
//bool Application::IsRunning()
//{
//	return isAppRunning;
//}
//
//Camera * Application::GetMainCamera()
//{
//	return MainCamera;
//}
//
//Application::Application(float Width, float Height)
//{
//	numInstances++;
//	windowWidth = Width;
//	windowHeight = Height;
//	MainCamera = new Camera(glm::vec3(0.0f, 0.0f, 3.0f));
//	deltaTime = 0;
//	GlInit();
//}
//
//void Application::GlInit()
//{
//	glfwInit();
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//
//	// glfw window creation
//	// --------------------
//	window = glfwCreateWindow(windowWidth, windowHeight, "Next Engine", NULL, NULL);
//	if (window == NULL)
//	{
//		DEBUG_LOG("GLFW", LOG_ERROR, "Failed to create GLFW window");
//		glfwTerminate();
//		return;
//	}
//	glfwMakeContextCurrent(window);
//	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//	glfwSetCursorPosCallback(window, mouse_callback);
//	glfwSetScrollCallback(window, scroll_callback);
//
//	glfwSetWindowUserPointer(window, this);
//	glfwSetWindowSizeCallback(window, resize_callback);
//	glfwSetKeyCallback(window, key_callback);
//
//	// tell GLFW to capture our mouse
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR);
//
//	// glad: load all OpenGL function pointers
//	// ---------------------------------------
//	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//	{
//		DEBUG_LOG("GLAD", LOG_ERROR, "Failed to initialize GLAD");
//		return;
//	}
//
//
//
//	glEnable(GL_DEPTH_TEST);
//	// build and compile shaders
//	// -------------------------
//	Shader shaderGeometryPass("../assets/shaders/8.2.g_buffer.vs", "../assets/shaders/8.2.g_buffer.fs");
//	Shader shaderLightingPass("../assets/shaders/8.2.deferred_shading.vs", "../assets/shaders/8.2.deferred_shading.fs");
//	//Shader shaderLightBox("8.2.deferred_light_box.vs", "8.2.deferred_light_box.fs");
//
//	MainPipeline.insert(std::make_pair("geometry_pass", shaderGeometryPass));
//	MainPipeline.insert(std::make_pair("lighting_pass", shaderLightingPass));
//	//MainPipeline.insert(std::make_pair("geometry_pass", shaderGeometryPass));
//
//
//
//
//
//	// configure g-buffer framebuffer
//	// ------------------------------
//
//	glGenFramebuffers(1, &gBuffer);
//	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
//
//	// position color buffer
//	glGenTextures(1, &gPosition);
//	glBindTexture(GL_TEXTURE_2D, gPosition);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, GetPercentOf(60, windowWidth), GetPercentOf(70, windowHeight), 0, GL_RGB, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
//	// normal color buffer
//	glGenTextures(1, &gNormal);
//	glBindTexture(GL_TEXTURE_2D, gNormal);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, GetPercentOf(60, windowWidth), GetPercentOf(70, windowHeight), 0, GL_RGB, GL_FLOAT, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
//	// color + specular color buffer
//	glGenTextures(1, &gAlbedoSpec);
//	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, GetPercentOf(60, windowWidth), GetPercentOf(70, windowHeight), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
//	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
//	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
//	glDrawBuffers(3, attachments);
//	// create and attach depth buffer (renderbuffer)
//	unsigned int rboDepth;
//	glGenRenderbuffers(1, &rboDepth);
//	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GetPercentOf(60, windowWidth), GetPercentOf(70, windowHeight));
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
//	// finally check if framebuffer is complete
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//		std::cout << "Framebuffer not complete!" << std::endl;
//	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//
//
//
//
//
//	glGenFramebuffers(1, &FramebufferName);
//	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
//
//	// The texture we're going to render to
//	
//	glGenTextures(1, &renderedTexture);
//
//	// "Bind" the newly created texture : all future texture functions will modify this texture
//	glBindTexture(GL_TEXTURE_2D, renderedTexture);
//
//	// Give an empty image to OpenGL ( the last "0" means "empty" )
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GetPercentOf(60, windowWidth), GetPercentOf(70, windowHeight), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
//
//	// Poor filtering
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//
//	// The depth buffer
//	
//	glGenRenderbuffers(1, &depthrenderbuffer);
//	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, GetPercentOf(60, windowWidth), GetPercentOf(70, windowHeight));
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
//
//
//	// Set "renderedTexture" as our colour attachement #0
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
//
//
//
//
//
//
//	// lighting info
//	// -------------
//
//	srand(13);
//	for (unsigned int i = 0; i < NR_LIGHTS; i++)
//	{
//		// calculate slightly random offsets
//		float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
//		float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
//		float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
//		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
//		// also calculate random color
//		float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
//		float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
//		float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
//		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
//	}
//
//
//
//
//	// shader configuration
//	// --------------------
//	shaderLightingPass.use();
//	shaderLightingPass.setInt("gPosition", 0);
//	shaderLightingPass.setInt("gNormal", 1);
//	shaderLightingPass.setInt("gAlbedoSpec", 2);
//
//
//}
//
//
//
//
//// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
//// ---------------------------------------------------------------------------------------------------------
//void Application::processInput(GLFWwindow *window)
//{
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//		MainCamera->ProcessKeyboard(FORWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//		MainCamera->ProcessKeyboard(BACKWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//		MainCamera->ProcessKeyboard(LEFT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//		MainCamera->ProcessKeyboard(RIGHT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
//		MainCamera->ProcessKeyboard(UP, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
//		MainCamera->ProcessKeyboard(DOWN, deltaTime);
//
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, GLFW_TRUE);
//}
//
//void Application::ResizeWindow(uint32 width, uint32 height)
//{
//	windowHeight = height;
//	windowWidth = width;
//}
//
//void Application::DisplayEntyties(ECS& ecs)
//{
//	//HACK HACK HACK
//	Array<EntityHandle> entities;
//	entities = ecs.GetEntities();
//	for (int i = 0; i < entities.size(); i++)
//	{
//		DEBUG_LOG_TEMP(ecs.GetComponent<RenderComponent>(entities[i])->mesh.c_str());
//	}
//
//}
//
//void Application::loadEntityToScene(ECS& ecs, string name)
//{
//	TransformComponent transformComponent;
//	transformComponent.transform.position = vec3f(0.f, 0.f, 0.f);
//	transformComponent.transform.scale = vec3f(1.f, 1.f, 1.f);
//	RenderComponent renderComponent;
//	renderComponent.diffuse = vec4f(0.2f, 0.4f, 0.5f, 1.f);
//	renderComponent.bIsVisible = true;
//	renderComponent.bIsWireFrame = true;
//	renderComponent.shader = "basic";
//	renderComponent.mesh = name;
//	renderComponent.meshPath = MapModelPaths.find(name)->second;
//
//	EntityHandle entity = ecs.MakeEntity(renderComponent, transformComponent);
//	cSceneManager::currentScene.sceneObjects.push_back(entity);
//}
//
//void Application::GUI_ShowMenuBar(ECS& ecs)
//{
//	// Menu Bar
//	ImGui::BeginMenuBar();
//	if (ImGui::BeginMenu("Menu"))
//	{
//		ImGui::MenuItem("(dummy menu)", NULL, false, false);
//		if (ImGui::MenuItem("New")) {}
//		if (ImGui::MenuItem("Open", "Ctrl+O")) {}
//		if (ImGui::BeginMenu("Open Recent"))
//		{
//			ImGui::MenuItem("fish_hat.c");
//			ImGui::MenuItem("fish_hat.inl");
//			ImGui::MenuItem("fish_hat.h");
//			if (ImGui::BeginMenu("More.."))
//			{
//				ImGui::MenuItem("Hello");
//				ImGui::MenuItem("Sailor");
//				if (ImGui::BeginMenu("Recurse.."))
//				{
//					//ShowExampleMenuFile();
//					//ImGui::EndMenu();
//				}
//				ImGui::EndMenu();
//			}
//			ImGui::EndMenu();
//		}
//		if (ImGui::MenuItem("Save Scene", "Ctrl+S")) 
//		{                          // Buttons return true when clicked (most widgets return true when edited/activated)
//			cSceneManager::SaveScene("test", ecs, *GetMainCamera());
//		}
//		if (ImGui::MenuItem("Load Scene", "Ctrl+L"))
//		{
//
//			cSceneManager::LoadScene("test", ecs, *GetMainCamera());
//	
//		}
//
//		if (ImGui::MenuItem("Save As..")) {}
//		ImGui::Separator();
//		if (ImGui::BeginMenu("Options"))
//		{
//			static bool enabled = true;
//			ImGui::MenuItem("Enabled", "", &enabled);
//			ImGui::BeginChild("child", ImVec2(0, 60), true);
//			for (int i = 0; i < 10; i++)
//				ImGui::Text("Scrolling Text %d", i);
//			ImGui::EndChild();
//			static float f = 0.5f;
//			static int n = 0;
//			static bool b = true;
//			ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
//			ImGui::InputFloat("Input", &f, 0.1f);
//			ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
//			ImGui::Checkbox("Check", &b);
//			ImGui::EndMenu();
//		}
//		ImGui::EndMenu();
//	}
//	if (ImGui::BeginMenu("Examples"))
//	{
//		ImGui::MenuItem("Main menu bar", NULL/*, &show_app_main_menu_bar*/);
//		ImGui::MenuItem("Console", NULL/*, &show_app_console*/);
//		ImGui::MenuItem("Log", NULL/*, &show_app_log*/);
//		ImGui::EndMenu();
//	}
//	if (ImGui::BeginMenu("Tools"))
//	{
//		ImGui::MenuItem("Metrics", NULL/*, &show_app_metrics*/);
//		ImGui::MenuItem("Style Editor", NULL/*, &show_app_style_editor*/);
//		ImGui::MenuItem("About Dear ImGui", NULL/*, &show_app_about*/);
//		ImGui::EndMenu();
//	}
//	ImGui::EndMenuBar();
//}
//
//
//
//
//
//
//void Application::ShowContentWindow(ECS& ecs)
//{
//	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
//	ImGui::BeginChild("Content Window", ImVec2(0, GetPercentOf(30, windowHeight)), true/*, window_flags*/);
//
//	ImGui::SetNextWindowContentSize(ImVec2(100 * MapModelPaths.size(), 0.0f));
//	ImGui::BeginChild("##ScrollingRegion", ImVec2(0, ImGui::GetFontSize() * 20), false, ImGuiWindowFlags_HorizontalScrollbar);
//	ImGui::Columns(MapModelPaths.size());
//	int ITEMS_COUNT = 1;
//	ImGuiListClipper clipper(ITEMS_COUNT);  // Also demonstrate using the clipper for large list
//	while (clipper.Step())
//	{
//		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
//		{
//			int j = 0;
//			for (const auto& x : MapModelPaths)
//			{
//
//				ImGui::PushID(j);
//				int frame_padding = -1 + i;     // -1 = uses default padding
//				//ImGui::Image((void*)(intptr_t)TestTexId, ImVec2(400/5, 400/5));
//				if (ImGui::ImageButton((void*)(intptr_t)TestTexId, ImVec2(400 / 5, 400 / 5)))
//				{
//					loadEntityToScene(ecs, x.first);
//				}
//				ImGui::PopID();
//				ImGui::TextWrapped(x.first.c_str());
//				ImGui::NextColumn();
//
//				j++;
//			}
//		}
//	}
//	ImGui::Columns(1);
//	ImGui::EndChild();
//
//	ImGui::EndChild();
//	ImGui::PopStyleVar();
//}
//
//
//bool showInspector = false;
//EntityHandle selectedEntity;
//
//void Application::ShowSceneObjectList(ECS& ecs)
//{
//	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
//	ImGui::BeginChild("Scene Tree", ImVec2(GetPercentOf(20, windowWidth), GetPercentOf(70, windowHeight)), true/*, window_flags*/);
//	auto enties = ecs.GetEntities();
//
//	static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow 
//		| ImGuiTreeNodeFlags_OpenOnDoubleClick 
//		| ImGuiTreeNodeFlags_SpanAvailWidth;
//
//	static int selection_mask = (1 << 2);
//	int node_clicked = -1;
//	int i = 0;
//	for (const auto& entity : cSceneManager::currentScene.sceneObjects)
//	{
//		// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
//		ImGuiTreeNodeFlags node_flags = base_flags;
//		const bool is_selected = (selection_mask & (1 << i)) != 0;
//		if (is_selected)
//			node_flags |= ImGuiTreeNodeFlags_Selected;
//
//		RenderComponent* RC = ecs.GetComponent<RenderComponent>(entity);
//		if (RC)
//		{
//			// Items 0..2 are Tree Node
//			bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, RC->mesh.c_str(), i);
//			if (ImGui::IsItemClicked())
//			{
//				showInspector = true;
//				node_clicked = i;
//				selectedEntity = entity;
//			}
//				
//			if (node_open)
//			{
//				ImGui::BulletText("Child1"); 
//				ImGui::BulletText("Child2");
//				ImGui::TreePop();
//			}
//		}
//		i++;
//	}
//	if (node_clicked != -1)
//	{
//		// Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
//		if (ImGui::GetIO().KeyCtrl)
//			selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
//		else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
//			selection_mask = (1 << node_clicked);           // Click to single-select
//	}
//	
//	ImGui::EndChild();
//	ImGui::PopStyleVar();
//
//}
//
//void Application::ShowInspector(ECS& ecs)
//{
//	//Inspector
//	if (showInspector)
//	{
//		TransformComponent* TC = ecs.GetComponent<TransformComponent>(selectedEntity);
//		if (TC)
//		{
//			//position
//			float PosVec[3];
//			for (int i = 0; i < 3; i++)
//			{
//				PosVec[i] = TC->transform.position[i];
//			}
//			//Scale
//			float scale = TC->transform.scale.x;
//
//
//
//			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 1.0f);
//			ImGui::BeginChild("Inspector", ImVec2(GetPercentOf(20, windowWidth) - 45, GetPercentOf(70, windowHeight)), true/*, window_flags*/);
//
//			ImGui::DragFloat3("Position", PosVec, 0.01f);
//			ImGui::DragFloat("Scale", &scale, 0.01f, 0.001f, 1000.0f);
//
//			//position
//			for (int i = 0; i < 3; i++)
//				TC->transform.position[i] = PosVec[i];
//			//scale
//			for (int i = 0; i < 3; i++)
//				TC->transform.scale[i] = scale;
//
//
//			if (ImGui::Button("Remove Object"))
//			{
//				cSceneManager::RemoveObjectFromScene(selectedEntity, ecs);
//				showInspector = false;
//			}
//			if (ImGui::Button("Close"))
//			{
//				showInspector = false;
//			}
//			ImGui::EndChild();
//			ImGui::PopStyleVar();
//		}
//
//
//	}
//}
//
//void Application::ShowEditor(GLint EditorTex)
//{
//	ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 1.0f);
//	ImGui::BeginChild("ViewPort", ImVec2(GetPercentOf(60, windowWidth), GetPercentOf(70, windowHeight)), true/*, window_flags*/);
//
//	ImGui::Image((void *)EditorTex, ImVec2(GetPercentOf(60, windowWidth), GetPercentOf(70, windowHeight)), ImVec2(0, 1), ImVec2(1, 0));
//	ImGui::EndChild();
//	ImGui::PopStyleVar();
//}
//
//
//// glfw: whenever the mouse moves, this callback is called
//// -------------------------------------------------------
//void Application::mouse_callback(GLFWwindow* window, double xpos, double ypos)
//{
//	
//	if (firstMouse)
//	{
//		lastX = xpos;
//		lastY = ypos;
//		firstMouse = false;
//	}
//
//	gXoffset = xpos - lastX;
//	gYoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//	lastX = xpos;
//	lastY = ypos;
//	Application::GetMainCamera()->ProcessMouseMovement(gXoffset, gYoffset);
//}
//
////// glfw: whenever the mouse scroll wheel scrolls, this callback is called
////// ----------------------------------------------------------------------
//void Application::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//	gScrollOffset = yoffset;
//}
//
//
//// glfw: whenever the window size changed (by OS or user resize) this callback function executes
//// ---------------------------------------------------------------------------------------------
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//	// make sure the viewport matches the new window dimensions; note that width and 
//	// height will be significantly larger than specified on retina displays.
//	glViewport(0, 0, width, height);
//}
//
//
//void resize_callback(GLFWwindow * window, int width, int height)
//{
//	Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
//	app->ResizeWindow(width, height);
//}
//
//
//bool cursor = true;
//void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
//{
//	Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
//	app->processInput(window);
//	//LOAD MODELS
//	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
//	{
//		app->GetMainCamera()->ToggleControls();
//		cursor = !cursor;
//		glfwSetInputMode(window, GLFW_CURSOR, cursor ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
//	}
//	
//}
//
