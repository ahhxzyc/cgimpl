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

public:
    GLFWwindow* glfwWindow_;

    int cursorX_ = 0, cursorY_ = 0;
    int relativeCursorX_ = 0, relativeCursorY_ = 0;

};

YCU_OPENGL_END