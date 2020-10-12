#include "platform/PlatformWindow.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include "rendering/Texture.h"
#include "rendering/RenderDevice.h"
#include "rendering/RenderTarget.h"
#include "rendering/Sampler.h"
#include "rendering/Window.h"
#include "rendering/Shader.h"
#include <Editor/EditorRenderContext.h>
#include <Core/Systems/RenderSystem.h>
#include "common/CommonTypes.h"
#include "rendering/AssetLoader.h"
#include <fstream>
#include <sstream>

void processInput(GLFWwindow *window);

bool loadTextFileWithIncludes(String& output, const String& fileName,
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

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 700;



#ifdef __APPLE__
	String TEST_TEXTURE_FILE = "/Users/nyan/Desktop/NxEngine_OLD/res/textures/stmpnk.jpg";
    String SHADER_TEXT_FILE = "/Users/nyan/Desktop/NxEngine_OLD/res/shaders/basicShader.glsl";
    String TEST_MODEL_FILE = "/Users/nyan/Desktop/NxEngine_OLD/res/models/monkey3.obj";
#else
   String TEST_TEXTURE_FILE = "../res/textures/stmpnk.jpg";
   String SHADER_TEXT_FILE = "../res/shaders/basicShader.glsl";
   //String TEST_MODEL_FILE = "../res/models/tinycube.obj";
   String TEST_MODEL_FILE = "../res/models/monkey3.obj";
#endif


int main()
{
    Window window(SCR_WIDTH, SCR_HEIGHT, "Test!");


    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window.GetWindowHandle(), true);
    ImGui_ImplOpenGL3_Init("#version 410");
    ImGui::StyleColorsDark();


    //Testing Texture
    RenderDevice renderDevice(window);
    Sampler sampler(renderDevice, SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR);


	int ha = window.GetHeight();
	int wa= window.GetWidth();
    mat4 projection = glm::perspective(glm::radians(45.0f), (float)window.GetWidth() / (float)window.GetHeight(), 0.1f, 100.0f);

    DrawParams drawParams;
	drawParams.primitiveType = PRIMITIVE_TRIANGLES;
	drawParams.faceCulling = FACE_CULL_FRONT;
	drawParams.shouldWriteDepth = true;
	drawParams.depthFunc = DRAW_FUNC_LESS;
//	drawParams.sourceBlend = RenderDevice::BLEND_FUNC_ONE;
//	drawParams.destBlend = RenderDevice::BLEND_FUNC_ONE;

    RenderTarget target(renderDevice);
    


    Array<IndexedModel> models;
	Array<uint32> modelMaterialIndices;
	Array<Material> modelMaterials;
    AssetLoader::LoadModels(TEST_MODEL_FILE, models, modelMaterialIndices, modelMaterials);
    VertexArray vertexArray(renderDevice, models[0], USAGE_STATIC_DRAW);
    
    ArrayBitmap testBitmap;
	testBitmap.Load(TEST_TEXTURE_FILE);
    Texture testtex(renderDevice, testBitmap, PixelFormat::FORMAT_RGBA, false, false);
    
    uint32 dbgTex = AssetLoader::TextureFromFile(TEST_TEXTURE_FILE);
    
    String shaderText;
    loadTextFileWithIncludes(shaderText, SHADER_TEXT_FILE, "#include");
    //StringFuncs::loadTextFileWithIncludes(shaderText, "./res/shaders/basicShader.glsl", "#include");
    Shader shader(renderDevice, shaderText);
    EditorRenderContext EditorContext(renderDevice, target, drawParams, shader, sampler, projection);

	//ECS
	ECS ecs;

	//ECS

	RenderableMeshComponent renderableMesh;
	renderableMesh.vertexArray = &vertexArray;
	renderableMesh.texture = &testtex;
	TransformComponent transformComp;
	transformComp.transform.position = vec3(0.9f, -0.15f, -40.0f);
	//transformComp.transform.rotation = vec3(5.9f, -0.15f, -50.0f);
	transformComp.transform.scale = vec3(7.0f);

	ecs.MakeEntity(transformComp, renderableMesh);


	RenderableMeshSystem renderSystem(EditorContext);
	SystemList systemList;
	systemList.AddSystem(renderSystem);
    
    while (!window.ShouldClose())
    {
        // input
        // -----
        processInput(window.GetWindowHandle());
        
            // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        ImGui::ShowDemoWindow();
        
		ImVec2 sizeP(250, 350);
        ImVec2 sizeW(400, 400);
        ImGui::SetNextWindowSize(sizeW);
        ImGui::Begin("TestImageWindow");
        ImGui::Image((void*)testtex.GetId(), sizeP);
        ImGui::End();

        // render
        // ------
		EditorContext.Clear(glm::vec4(0.576, 0.439, 0.859, 0), true);

		//glm::mat4 trans = glm::mat4(1.0f);
		//trans = glm::translate(trans, glm::vec3(5.9f, -0.15f, -50.0f));
		//trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.01f, 0.0f));
	 //   trans = glm::scale(trans, glm::vec3(8.1f, 8.1f, 8.1f));

        //EditorContext.RenderMesh(vertexArray, testtex, trans);
		ecs.UpdateSystems(systemList, 0.0f);

        EditorContext.Flush();

		

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        //
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.Present();
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


