#include "window.h"

#include <ycu/log/log.h>
#include <iostream>

YCU_OPENGL_BEGIN

Window::Window()
{
    // init GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to create glfw window." << std::endl;
    }
    glfwWindow_ = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!glfwWindow_)
    {
        std::cout << "Failed to create glfw window." << std::endl;
    }
    glfwMakeContextCurrent(glfwWindow_);

    // lock mouse cursor
    glfwSetInputMode(glfwWindow_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // init GLAD
    if (!gladLoadGLLoader((GLADloadproc(glfwGetProcAddress))))
    {
        std::cout << "Failed to init GLAD" << std::endl;
    };

    // register GLFW callbacks
    //glfwSetKeyCallback(m_GlfwWindow, [](GLFWwindow*, int key, int, int action, int)
    //    {
    //        Window->GetEventSender()->Send(KeyDownEvent(glfw_to_ycu_keycode(key)));
    //    });
}


Window::~Window()
{
    glfwTerminate();
    glfwWindow_ = nullptr;
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(glfwWindow_);
}

void Window::DoEvents()
{
    glfwPollEvents();

    // update cursor position
    double xpos, ypos;
    glfwGetCursorPos(glfwWindow_, &xpos, &ypos);
    relativeCursorX_ = int(xpos - cursorX_);
    relativeCursorY_ = int(ypos - cursorY_);
    cursorX_ = int(xpos);
    cursorY_ = int(ypos);
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(glfwWindow_);
}


YCU_OPENGL_END