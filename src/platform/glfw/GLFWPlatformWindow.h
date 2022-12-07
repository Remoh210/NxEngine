#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <Core/Camera/Camera.h>
#include "Common/Common.h"
#include "Common/Input/InputTypes.h"

#include <functional>

//typedef void(*CursorCallbackFunc)(int xpos, int ypos);

typedef GLFWwindow* WindowHandle;

class GLFWPlatformWindow
{
public:

    GLFWPlatformWindow(uint32 width, uint32 height, const char* title);
    ~GLFWPlatformWindow() {};
    void Resize(int width, int height);
    //void ChangeTitle(const char* title);

	void UpdateInput();
    void Present();
    inline bool ShouldClose() { return glfwWindowShouldClose(window); };
    inline WindowHandle GetWindowHandle() { return window; };
    inline uint32 GetHeight() { return mHeight; }
    inline uint32 GetWidth() { return mWidth; }

	static float GetHorizontalInputAxis() 
	{ 
		return horizontalInputAxis; 
	};
	static float GetVerticalInputAxis()   
	{ 
		return verticalInputAxis; 
	};

	static bool GetKeyPressed(InputKey keyIn) { return keyIn == keyPressed; };

	//inline void SetMSAA(uint32 numSamples) { glfwWindowHint(GLFW_SAMPLES, numSamples); }

	inline void SetMouseCallback(std::function<void (int, int)> callbackFunc) { MouseCallbackFunc  = callbackFunc; }
    inline void SetFrameBufferResizeCallback(std::function<void (int, int)> callbackFunc) { FrameBufferResizeCallbackBackFunc  = callbackFunc; }
	inline void SetMouseScrollCallback(std::function<void (float)> callbackFunc) { MouseScrollCallbackFunc = callbackFunc; }

	static Camera* mainCamera;

    static std::function<void (int, int)> MouseCallbackFunc;
	static std::function<void(float yoffset)> MouseScrollCallbackFunc;
    static std::function<void (int, int)> FrameBufferResizeCallbackBackFunc;


	static void GLFWMouseCallback(GLFWwindow* window, double xpos, double ypos);
	static void GLFWFrameBufferResizeCallback(GLFWwindow* window, int width, int height);
	static void GLFWMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	

private:
    WindowHandle window;
    int mWidth;
    int mHeight;

	static float lastX;
	static float lastY;
	static bool bFirstMouse;

	static float horizontalInputAxis;
	static float verticalInputAxis;
	static InputKey keyPressed;
	
	void ProcessKeyInput();
};
