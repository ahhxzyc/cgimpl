#pragma once

#include "common.h"
#include "../event/event.h"
#include "../event/keyboard.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

YCU_OPENGL_BEGIN

class Window : public ycu::event::sender_t<
    ycu::event::KeyDownEvent, ycu::event::KeyUpEvent, ycu::event::KeyHoldEvent>
{
public:
    Window();
    ~Window();

    bool ShouldClose();
    void DoEvents();
    void SwapBuffers();

    int width();
    int height();

    void capture_mouse();
    void release_mouse();

public:
    GLFWwindow* glfwWindow_;

    int cursorX_ = 0, cursorY_ = 0;
    int relativeCursorX_ = 0, relativeCursorY_ = 0;
};

YCU_OPENGL_END