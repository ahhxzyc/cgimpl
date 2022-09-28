#pragma once

#define YCU_OPENGL_BEGIN namespace ycu::opengl {
#define YCU_OPENGL_END }

#define NOMINMAX
#include <glad/glad.h>

YCU_OPENGL_BEGIN

bool logCall(const char *function, const char *file, const int line);
void clearError();

#if defined(DEBUG) | defined(_DEBUG)
#	define GLCALL(x)                                \
		ycu::opengl::clearError();                         \
		x;                                           \
		if (!ycu::opengl::logCall(#x, __FILE__, __LINE__)) \
		{}
#else
#	define GLCALL(x) x
#endif


YCU_OPENGL_END