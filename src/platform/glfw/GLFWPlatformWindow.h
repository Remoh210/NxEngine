#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <Core/Camera/Camera.h>
#include "common/Common.h"

#include <functional>

typedef void(*CursorCallbackFunc)(int xpos, int ypos);

typedef GLFWwindow* WindowHandle;

class GLFWPlatformWindow
{
public:

    GLFWPlatformWindow(uint32 width, uint32 height, const char* title);
    ~GLFWPlatformWindow() {};
    void Resize(int width, int height);
    void ChangeTitle(const char* title);

    void Present();
    inline bool ShouldClose() { return glfwWindowShouldClose(window); };
    inline WindowHandle GetWindowHandle() { return window; };
    inline uint32 GetHeight() { return mHeight; }
    inline uint32 GetWidth() { return mWidth; }

	inline void SetMouseCallback(CursorCallbackFunc callbackFunc) { MouseCallBackFunc  = callbackFunc; }

	static Camera* mainCamera;

	static CursorCallbackFunc MouseCallBackFunc;
private:
    WindowHandle window;
    int mWidth;
    int mHeight;

	
	static void GLFWMouseCallback(GLFWwindow* window, double xpos, double ypos);
};
