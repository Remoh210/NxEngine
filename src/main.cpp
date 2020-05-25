#include "platform/PlatformWindow.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <stb_image.h>
#include "rendering/Texture.h"
#include "rendering/RenderDevice.h"
#include "rendering/RenderTarget.h"
#include "rendering/Sampler.h"
#include "rendering/Window.h"
#include "rendering/Shader.h"
#include "EditorRenderContext.h"
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

unsigned int TextureFromFile(String path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    uint8 *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;



#ifdef __APPLE__
	String TEST_TEXTURE_FILE = "/Users/nyan/Desktop/Workspace/NxEngine/res/textures/stmpnk2.png";
    String SHADER_TEXT_FILE = "/Users/nyan/Desktop/Workspace/NxEngine/res/shaders/basicShader.glsl";
    String TEST_MODEL_FILE = "/Users/nyan/Desktop/Workspace/NxEngine/res/models/nanosuit/nanosuit.obj";
#else
   String TEST_TEXTURE_FILE = "../res/textures/stmpnk.jpg";
   String SHADER_TEXT_FILE = "../res/shaders/basicShader.glsl";
   String TEST_MODEL_FILE = "../res/models/nanosuit/nanosuit.obj";
#endif




const char *vertexShaderSource ="#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "out vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "   ourColor = aColor;\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(ourColor, 1.0f);\n"
    "}\n\0";

int main()
{
    Window window(800, 600, "Test!");
    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top

    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);

    // as we only have a single shader, we could also just activate our shader once beforehand if we want to
    glUseProgram(shaderProgram);

    // render loop
    // -----------
    ImGui::CreateContext();
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    ImGui_ImplGlfw_InitForOpenGL(window.GetWindowHandle(), true);
    ImGui_ImplOpenGL3_Init("#version 410");
    ImGui::StyleColorsDark();


    //Testing Texture
    RenderDevice renderDevice(window);
    Sampler sampler(renderDevice, SamplerFilter::FILTER_LINEAR_MIPMAP_LINEAR);

    String shaderText;
    loadTextFileWithIncludes(shaderText, SHADER_TEXT_FILE, "#include");
	//StringFuncs::loadTextFileWithIncludes(shaderText, "./res/shaders/basicShader.glsl", "#include");
	Shader shader(renderDevice, shaderText);
    mat4 projection = glm::perspective(glm::radians(45.0f), (float)window.GetWidth() / (float)window.GetHeight(), 0.1f, 100.0f);

    DrawParams drawParams;
	drawParams.primitiveType = PRIMITIVE_TRIANGLES;
	drawParams.faceCulling = FACE_CULL_BACK;
	drawParams.shouldWriteDepth = true;
	drawParams.depthFunc = DRAW_FUNC_LESS;
//	drawParams.sourceBlend = RenderDevice::BLEND_FUNC_ONE;
//	drawParams.destBlend = RenderDevice::BLEND_FUNC_ONE;

    RenderTarget target(renderDevice);
    EditorRenderContext EditorContext(renderDevice, target, drawParams, shader, sampler, projection);

    Array<IndexedModel> models;
	Array<uint32> modelMaterialIndices;
	Array<Material> modelMaterials;
    AssetLoader::LoadModels(TEST_MODEL_FILE, models, modelMaterialIndices, modelMaterials);
    VertexArray vertexArray(renderDevice, models[0], USAGE_STATIC_DRAW);

    ArrayBitmap testBitmap;
	testBitmap.Load(TEST_TEXTURE_FILE);
    Texture testtex(renderDevice, testBitmap, PixelFormat::FORMAT_RGBA, false, false);
    
    uint32 dbgTex = TextureFromFile(TEST_TEXTURE_FILE);
    std::cout << (uint32)-1;

    //Asset load test
    //RenderDevice device()
	//Sampler sampler(device, RenderDevice::FILTER_LINEAR_MIPMAP_LINEAR);
    
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
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        EditorContext.RenderMesh(vertexArray, testtex, mat4());
        EditorContext.Flush();
        // render the triangle
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        //
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.Present();
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

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


