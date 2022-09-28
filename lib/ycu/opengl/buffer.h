#pragma once

#include <ycu/opengl/common.h>

YCU_OPENGL_BEGIN

enum class BufferType
{
    Null, VERTEX, INDEX
};

class Buffer
{
public:
    Buffer(BufferType type);
    ~Buffer();
    
    void Bind();
    void UnBind();
    void Upload(const void *start, int numBytes);
    //void Download(void *start, int numBytes);

private:
    GLint GLBufferType();

private:
    BufferType m_Type;
    GLuint m_Handle;
};

YCU_OPENGL_END