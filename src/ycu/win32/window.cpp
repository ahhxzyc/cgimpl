#include "window.h"

#include <unordered_map>

YCU_BEGIN

static std::unordered_map<HWND, Window*> s_WindowHandleToObject;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

Window::Window(const WindowSpec &spec)
    :m_Width(spec.width), m_Height(spec.height)
{
    // register window class
    m_HInst = GetModuleHandle(nullptr);
    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = m_HInst;
    wc.lpszClassName = s_WindowClassName;
    RegisterClass(&wc);

    // create window
    m_Handle = CreateWindow(
        s_WindowClassName, spec.title.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, m_Width, m_Height,
        NULL, NULL, m_HInst,  NULL);
    
    ShowWindow(m_Handle, SW_SHOW);

    s_WindowHandleToObject[m_Handle] = this;
}

Window::~Window()
{
    s_WindowHandleToObject.erase(m_Handle);
}

void Window::OnMsgClose()
{
    m_ShouldClose = true;
    DestroyWindow(m_Handle);
    UnregisterClass(s_WindowClassName, m_HInst);
}

void Window::DoEvents()
{
    MSG msg = {};
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto it = s_WindowHandleToObject.find(hwnd);
    if (it != s_WindowHandleToObject.end())
    {
        auto& window = *it->second;
        switch (uMsg)
        {
            case WM_CLOSE:
                window.OnMsgClose();
                break;
            default:
                ;// do nothing
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

YCU_END