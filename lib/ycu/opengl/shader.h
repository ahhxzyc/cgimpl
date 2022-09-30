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

    void bind()     const;
    void unbind()   const;

    void set_uniform(const std::string &name, const ycu::math::mat4f &mat);
    void set_uniform(const std::string &name, const ycu::math::float3 &vec);
    void set_uniform(const std::string &name, float val);
    void set_uniform(const std::string &name, int val);
public:
    std::string name;
private:
    GLuint m_Handle = 0;
};

YCU_OPENGL_END