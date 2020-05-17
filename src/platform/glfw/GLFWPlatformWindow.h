#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>


typedef GLFWwindow* WindowHandle;

class GLFWPlatformWindow
{
public:
    GLFWPlatformWindow(int width, int height, const char* title);
    //~GLFWPlatformWindow();
    void Resize(int width, int height);
    void ChangeTitle(const char* title);

    void Present();
    inline bool ShouldClose() { return glfwWindowShouldClose(window); };
    inline WindowHandle GetWindowHandle(){ return window; };

private:
    WindowHandle window;
    int width;
    int height;
};
