#include "framebuffer.h"
#include "texture.h"
#include "../log/log.h"

YCU_OPENGL_BEGIN


Framebuffer::Framebuffer(int w, int h) : width_(w), height_(h)
{
    GLCALL(glCreateFramebuffers(1, &handle_));

    // render target
    add_render_target(GL_RGBA8);

    // depth & stencil
    GLuint ds;
    GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &ds));
    GLCALL(glBindTexture(GL_TEXTURE_2D, ds));
    GLCALL(glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, w, h));
    GLCALL(glNamedFramebufferTexture(handle_, GL_DEPTH_STENCIL_ATTACHMENT, ds, 0));

    ASSERT(is_complete());
}

Framebuffer::~Framebuffer()
{
    GLCALL(glDeleteFramebuffers(1, &handle_));
}



void Framebuffer::bind()
{
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, handle_));
    specify_drawbuffers();
}

void Framebuffer::unbind()
{
    GLCALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void Framebuffer::add_render_target(GLenum format)
{
    auto tex = std::make_shared<Texture>(
        TextureDesc::without_mipmap(width_, height_, format));
    renderTargets_.push_back(tex);
    GLCALL(glNamedFramebufferTexture(
        handle_, GL_COLOR_ATTACHMENT0 + renderTargets_.size() - 1, tex->handle(), 0));
    ASSERT(is_complete());
}

std::shared_ptr<Texture> Framebuffer::nth_render_target(int index)
{
    if (index < renderTargets_.size())
    {
        return renderTargets_[index];
    }
    return {};
}

bool Framebuffer::is_complete()
{
    GLCALL(auto result = glCheckNamedFramebufferStatus(handle_, GL_FRAMEBUFFER));
    return result == GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::specify_drawbuffers()
{
    GLuint attachments[8];
    int n = (std::min)(8, (int)renderTargets_.size());
    for (int i = 0; i < n; i ++)
    {
        attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }
    GLCALL(glNamedFramebufferDrawBuffers(handle_, n, attachments));
}

void Framebuffer::set_render_target(GLint cubeTexture, int face, int rtIndex, int level /*= 0*/)
{
    GLCALL(glNamedFramebufferTextureLayer(handle_, GL_COLOR_ATTACHMENT0 + rtIndex, cubeTexture, level, face));
}

YCU_OPENGL_END