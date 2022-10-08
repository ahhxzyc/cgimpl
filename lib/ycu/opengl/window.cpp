#include "window.h"
#include "../log/log.h"
#include "../event/keyboard.h"

#include <iostream>
#include <unordered_map>

YCU_OPENGL_BEGIN

using namespace ycu::event;

static std::unordered_map<GLFWwindow*, Window*> sWindowPtrMap;

Window::Window(int w, int h)
{
    // init GLFW
    ASSERT(glfwInit());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindow_ = glfwCreateWindow(w, h, "Hello World", NULL, NULL);
    ASSERT(glfwWindow_);
    glfwMakeContextCurrent(glfwWindow_);

    // lock mouse cursor
    capture_mouse();

    // init GLAD
    //ASSERT(gladLoadGLLoader((GLADloadproc(glfwGetProcAddress))));
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

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

int Window::width()
{
    int w, h;
    glfwGetWindowSize(glfwWindow_, &w, &h);
    return w;
}

int Window::height()
{
    int w, h;
    glfwGetWindowSize(glfwWindow_, &w, &h);
    return h;
}

void Window::capture_mouse()
{
    glfwSetInputMode(glfwWindow_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::release_mouse()
{
    glfwSetInputMode(glfwWindow_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

YCU_OPENGL_END