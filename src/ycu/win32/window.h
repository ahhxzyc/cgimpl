#include <string>
#include <windows.h>

#define YCU_BEGIN namespace ycu {
#define YCU_END }

YCU_BEGIN

struct WindowSpec
{
    int width;
    int height;
    std::string title = "Untitled";
};

class Window
{
public:
    Window(const WindowSpec &spec);
    ~Window();

public:
    auto GetHandle() const {return m_Handle;}
    auto GetWidth() const {return m_Width;}
    auto GetHeight() const { return m_Height; }
    auto GetAspectRatio() const { return static_cast<float>(m_Width) / m_Height; }
    auto ShouldClose() const {return m_ShouldClose;}

    void OnMsgClose();
    
    void DoEvents();

private:
    HWND m_Handle{};
    int m_Width, m_Height;
    bool m_ShouldClose = false;

    inline static const char s_WindowClassName[] = "Win32WindowClass";
    HINSTANCE m_HInst;
};

YCU_END