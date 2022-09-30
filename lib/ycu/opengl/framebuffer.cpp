#include "framebuffer.h"
#include "texture.h"
#include "../log/log.h"

YCU_OPENGL_BEGIN


Framebuffer::Framebuffer(int w, int h) : width_(w), height_(h)
{
    GLCALL(glCreateFramebuffers(1, &handle_));

    // depth & stencil
    GLuint ds;
    GLCALL(glCreateTextures(GL_TEXTURE_2D, 1, &ds));
    GLCALL(glBindTexture(GL_TEXTURE_2D, ds));
    GLCALL(glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, w, h));
    GLCALL(glNamedFramebufferTexture(handle_, GL_DEPTH_STENCIL_ATTACHMENT, ds, 0));
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
    if (numRTs >= maxNumRTs)
    {
        LOG_WARN("Too many render targets");
        return;
    }
    auto tex = std::make_shared<Texture>(TextureDesc::without_mipmap(width_, height_, format));
    renderTargets_[numRTs] = tex;
    GLCALL(glNamedFramebufferTexture(handle_, GL_COLOR_ATTACHMENT0 + numRTs, tex->handle, 0));
    numRTs ++ ;
    ASSERT(is_complete());
}

std::shared_ptr<Texture> Framebuffer::nth_render_target(int index)
{
    if (index >= numRTs)
    {
        LOG_WARN("Error requesting render target {} from {} RTs", index, numRTs);
        return {};
    }
    if (!renderTargets_[index])
    {
        LOG_WARN("Unable to fetch render target {}, maybe it is a cubemap", index);
        return {};
    }
    return renderTargets_[index];
}

bool Framebuffer::is_complete()
{
    GLCALL(auto result = glCheckNamedFramebufferStatus(handle_, GL_FRAMEBUFFER));
    return result == GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::specify_drawbuffers()
{
    GLuint attachments[maxNumRTs];
    for (int i = 0; i < numRTs; i ++ )
    {
        attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }
    GLCALL(glNamedFramebufferDrawBuffers(handle_, numRTs, attachments));
}

void Framebuffer::set_render_target(int index, GLint cubeTexture, int face, int level /*= 0*/)
{
    if (index > numRTs)
    {
        LOG_WARN("Error setting render target {} among {} RTs, the index should be consecutive", index, numRTs);
        return ;
    }
    if (index == numRTs)
        numRTs ++ ;
    renderTargets_[index] = {};
    GLCALL(glNamedFramebufferTextureLayer(handle_, GL_COLOR_ATTACHMENT0 + index, cubeTexture, level, face));
    ASSERT(is_complete());
}

YCU_OPENGL_END