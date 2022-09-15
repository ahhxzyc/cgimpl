#include "window.h"

#include <iostream>

YCU_OPENGL_BEGIN


Window::Window()
{
    // init GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to create glfw window." << std::endl;
    }
    m_GlfwWindow = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!m_GlfwWindow)
    {
        std::cout << "Failed to create glfw window." << std::endl;
    }
    glfwMakeContextCurrent(m_GlfwWindow);

    // init GLAD
    if (!gladLoadGLLoader((GLADloadproc(glfwGetProcAddress))))
    {
        std::cout << "Failed to init GLAD" << std::endl;
    };
}


Window::~Window()
{
    glfwTerminate();
    m_GlfwWindow = nullptr;
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(m_GlfwWindow);
}

void Window::DoEvents()
{
    glfwPollEvents();
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(m_GlfwWindow);
}


YCU_OPENGL_END