#include "shader.h"
#include <ycu/log/log.h>

YCU_OPENGL_BEGIN

using ycu::math::float3;
using ycu::math::mat4f;

Shader::Shader(
    const std::string &name, const std::string &vertexSource, const std::string &fragmentSource)
{
    GLCALL(m_Handle = glCreateProgram());

    auto vs = compile(vertexSource, GL_VERTEX_SHADER);
    auto fs = compile(fragmentSource, GL_FRAGMENT_SHADER);

    // Link program
    int suc;
    char log[512];
    GLCALL(glLinkProgram(m_Handle));
    GLCALL(glGetProgramiv(m_Handle, GL_LINK_STATUS, &suc));
    if (!suc)
    {
        GLCALL(glGetProgramInfoLog(m_Handle, 512, NULL, log));
        LOG_ERROR("Failed to link shader: {}", log);
    }

    // Clean up
    GLCALL(glDetachShader(m_Handle, vs));
    GLCALL(glDetachShader(m_Handle, fs));
    GLCALL(glDeleteShader(vs));
    GLCALL(glDeleteShader(fs));
}

Shader::~Shader()
{
    GLCALL(glDeleteProgram(m_Handle));
}


GLuint Shader::compile(const std::string &code, GLuint type) const
{
    const char *cstr = code.c_str();
    auto sid = glCreateShader(type);
    GLCALL(glShaderSource(sid, 1, &cstr, nullptr));
    GLCALL(glCompileShader(sid));
    GLint result;
    GLCALL(glGetShaderiv(sid, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE)
    {
        GLint length;
        GLCALL(glGetShaderiv(sid, GL_INFO_LOG_LENGTH, &length));
        std::string error;
        error.resize(length);
        GLCALL(glGetShaderInfoLog(sid, length, &length, error.data()));
        GLCALL(glDeleteShader(sid));
        LOG_ERROR("Failed to compile shader. Error : {}", error);
        return 0;
    }
    GLCALL(glAttachShader(m_Handle, sid));
    return sid;
}

void Shader::Bind() const
{
    GLCALL(glUseProgram(m_Handle));
}
void Shader::Unbind()   const
{
    GLCALL(glUseProgram(0));
}

void Shader::SetUniform(const std::string &name, const mat4f &mat)
{
    GLCALL(glUniformMatrix4fv(glGetUniformLocation(m_Handle, name.c_str()), 1, GL_FALSE, &mat[0][0]));
}

void Shader::SetUniform(std::string const &name, const float3 &vec)
{
    GLCALL(glUniform3fv(glGetUniformLocation(m_Handle, name.c_str()), 1, &vec.x));
}

void Shader::SetUniform(std::string const &name, float val)
{
    GLCALL(glUniform1f(glGetUniformLocation(m_Handle, name.c_str()), val));
}

void Shader::SetUniform(std::string const &name, int val)
{
    GLCALL(glUniform1i(glGetUniformLocation(m_Handle, name.c_str()), val));
}


YCU_OPENGL_END