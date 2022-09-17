#include "window.h"
#include "../log/log.h"
#include "../event/keyboard.h"

#include <iostream>
#include <unordered_map>

YCU_OPENGL_BEGIN

using namespace ycu::event;

static std::unordered_map<GLFWwindow*, Window*> sWindowPtrMap;

Window::Window()
{
    // init GLFW
    ASSERT(glfwInit());
    glfwWindow_ = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    ASSERT(glfwWindow_);
    glfwMakeContextCurrent(glfwWindow_);

    // lock mouse cursor
    glfwSetInputMode(glfwWindow_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // init GLAD
    ASSERT(gladLoadGLLoader((GLADloadproc(glfwGetProcAddress))));

    // register GLFW callbacks
    sWindowPtrMap[glfwWindow_] = this;
    glfwSetKeyCallback(glfwWindow_, [](GLFWwindow *win, int key, int, int action, int)
        {
            auto window = sWindowPtrMap[win];
            if (!window)
                return;
            switch (action)
            {
                case GLFW_PRESS:
                    window->send(KeyDownEvent{key});
                    break;
                case GLFW_REPEAT:
                    window->send(KeyHoldEvent{key});
                    break;
                case GLFW_RELEASE:
                    window->send(KeyUpEvent{key});
                    break;
                default:
                    ;
            }
        });
}


Window::~Window()
{
    glfwTerminate();
    sWindowPtrMap.erase(glfwWindow_);
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