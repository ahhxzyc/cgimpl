#include "buffer.h"
#include <ycu/log/log.h>

YCU_OPENGL_BEGIN


Buffer::Buffer(BufferType type)
    : m_Type(type)
{
	GLCALL(glCreateBuffers(1, &m_Handle));
}

Buffer::~Buffer()
{
	ASSERT(m_Handle != 0);
	GLCALL(glDeleteBuffers(1, &m_Handle));
}


void Buffer::Bind()
{
    GLCALL(glBindBuffer(GLBufferType(), m_Handle));
}

void Buffer::UnBind()
{
	GLCALL(glBindBuffer(GLBufferType(), 0));
}


GLint Buffer::GLBufferType()
{
	GLint ret = 0;
	switch (m_Type)
	{
		case BufferType::VERTEX:
			ret = GL_ARRAY_BUFFER;
			break;
		case BufferType::INDEX:
			ret = GL_ELEMENT_ARRAY_BUFFER;
			break;
		default:
			LOG_ERROR("Unknown opengl buffer type {}", static_cast<int>(m_Type));
			break;
	}
	return ret;
}

void Buffer::Upload(const void *start, int numBytes)
{
	GLCALL(glNamedBufferData(m_Handle, numBytes, start, GL_STATIC_DRAW));
}

YCU_OPENGL_END