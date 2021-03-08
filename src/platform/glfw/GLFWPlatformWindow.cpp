#include "GLFWPlatformWindow.h"
#include "Core/Application/Settings/GlobalSettings.h"
#define LOG_TYPE_WINDOW "Window"

//Camera* GLFWPlatformWindow::MainCamera;

std::function<void (int, int)> GLFWPlatformWindow::MouseCallbackFunc = [](int, int){};
std::function<void (int, int)> GLFWPlatformWindow::FrameBufferResizeCallbackBackFunc = [](int, int){};
std::function<void(float yoffset)> GLFWPlatformWindow::MouseScrollCallbackFunc = [](float) {};

float GLFWPlatformWindow::lastX = 0;
float GLFWPlatformWindow::lastY = 0;
float GLFWPlatformWindow::horizontalInputAxis = 0;
float GLFWPlatformWindow::verticalInputAxis   = 0;
bool  GLFWPlatformWindow::bFirstMouse = true;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	int verMaj = GlobalSettings::GetAPIVersionMajor();
	int verMin = GlobalSettings::GetAPIVersionMinor();
	char title[50];
	sprintf(title, "OpenGL %d.%d [%dx%d]", verMaj, verMin, width, height);
	glfwSetWindowTitle(window, title);
    glViewport(0, 0, width, height);
}

void GLFWPlatformWindow::GLFWMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (bFirstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		bFirstMouse = false;
	}

	horizontalInputAxis = xpos - lastX;
	verticalInputAxis = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	GLFWPlatformWindow::MouseCallbackFunc(horizontalInputAxis, verticalInputAxis);
}

void GLFWPlatformWindow::GLFWFrameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
	int verMaj = GlobalSettings::GetAPIVersionMajor();
	int verMin = GlobalSettings::GetAPIVersionMinor();
	char title[50];
	sprintf(title, "OpenGL %d.%d [%dx%d]", verMaj, verMin, width, height);
	glfwSetWindowTitle(window, title);
	glViewport(0, 0, width, height);

    FrameBufferResizeCallbackBackFunc(width, height);
}

void GLFWPlatformWindow::GLFWMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	MouseScrollCallbackFunc(yoffset);
}

GLFWPlatformWindow::GLFWPlatformWindow(uint32 width, uint32 height, const char* title)
	:mWidth(width),
	mHeight(height)
{
    glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GlobalSettings::GetAPIVersionMajor());
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GlobalSettings::GetAPIVersionMinor());
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GL_FALSE);
    glfwWindowHint(GLFW_SAMPLES, 8);



#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_FALSE);
    //glfwWindowHint(GLFW_SCALE, GL_FALSE);
#endif

    // glfw window creation
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (window == NULL)
    {
        DEBUG_LOG(LOG_TYPE_WINDOW, LOG_ERROR, "Failed to create GLFW window");
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
	//TODO: Change this 
    glfwSetFramebufferSizeCallback(window, GLFWPlatformWindow::GLFWFrameBufferResizeCallback);
	glfwSetCursorPosCallback(window, GLFWPlatformWindow::GLFWMouseCallback);
	glfwSetScrollCallback(window, GLFWPlatformWindow::GLFWMouseScrollCallback);
    gladLoadGL(glfwGetProcAddress);
    int width_w, height_w;
    glfwGetFramebufferSize(window, &width_w, &height_w);
    
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_RED_BITS, 32);
	glfwWindowHint(GLFW_GREEN_BITS, 32);
	glfwWindowHint(GLFW_BLUE_BITS, 32);
	glfwWindowHint(GLFW_REFRESH_RATE, 60);

	bFirstMouse = false;
}

void GLFWPlatformWindow::Resize(int width, int height)
{
    
}

void GLFWPlatformWindow::Present()
{
    glfwSwapBuffers(window);
}




