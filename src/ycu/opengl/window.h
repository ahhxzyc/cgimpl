#pragma once

#include <ycu/opengl/common.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

YCU_OPENGL_BEGIN

class Window
{
public:
    Window();
    ~Window();

    bool ShouldClose();
    void DoEvents();
    void SwapBuffers();

private:
    GLFWwindow* m_GlfwWindow;
};

YCU_OPENGL_END