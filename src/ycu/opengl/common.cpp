#include "common.h"
#include <ycu/log/log.h>
#include <string>

YCU_OPENGL_BEGIN

bool logCall(const char *function, const char *file, const int line)
	{
		GLenum err(glGetError());
		bool   ret = true;
		while (err != GL_NO_ERROR)
		{
			std::string error;
			switch (err)
			{
#define STR(r)      \
	case GL_##r:    \
		error = #r; \
		break;
				STR(INVALID_OPERATION);
				STR(INVALID_ENUM);
				STR(INVALID_VALUE);
				STR(OUT_OF_MEMORY);
				STR(INVALID_FRAMEBUFFER_OPERATION);
#undef STR
				default:
					return "UNKNOWN_GL_ERROR";
			}
			LOG_ERROR("Error : {0}, File : {1}, Func : {2}, Line : {3}", error, file, function, line);
			ret = false;
			err = glGetError();
		}
		return ret;
	}

	void clearError()
	{
        while (glGetError() != GL_NO_ERROR)
            ;
	}

YCU_OPENGL_END