#pragma once

#include <ycu/opengl/common.h>
#include <ycu/math.h>
#include <string>

YCU_OPENGL_BEGIN

class Shader
{
public:
    Shader(const std::string &name, const std::string &vertexSource, const std::string &fragmentSource);
    ~Shader();
public:
    GLuint compile(const std::string &code, GLuint type) const;

    void Bind()     const;
    void Unbind()   const;

    void SetUniform(const std::string &name, const ycu::math::mat4f &mat);
    void SetUniform(std::string const &name, const ycu::math::float3 &vec);
    void SetUniform(std::string const &name, float val);
    void SetUniform(std::string const &name, int val);
private:
    GLuint m_Handle = 0;
};

YCU_OPENGL_END