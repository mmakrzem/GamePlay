/**
 * FrameBuffer.cpp
 */

#include "Base.h"
#include "FrameBuffer.h"

namespace gameplay
{

static unsigned int __maxRenderTargets = 0;
static std::vector<FrameBuffer*> __frameBuffers;

FrameBuffer::FrameBuffer(const char* id) :
    _id(id ? id : ""), _handle(0), _renderTargets(NULL), _depthStencilTarget(NULL)
{
}

FrameBuffer::~FrameBuffer()
{
    if (_renderTargets)
    {
        for (unsigned int i = 0; i < __maxRenderTargets; ++i)
        {
            SAFE_RELEASE(_renderTargets[i]);
        }
        SAFE_DELETE_ARRAY(_renderTargets);
    }

    // Release GL resource.
    if (_handle)
    {
        GL_ASSERT( glDeleteFramebuffers(1, &_handle) );
    }

    // Remove self from vector.
    std::vector<FrameBuffer*>::iterator it = std::find(__frameBuffers.begin(), __frameBuffers.end(), this);
    if (it != __frameBuffers.end())
    {
        __frameBuffers.erase(it);
    }
}

FrameBuffer* FrameBuffer::create(const char* id)
{
    // Create GL FBO resource.
    GLuint handle = 0;
    GL_ASSERT( glGenFramebuffers(1, &handle) );

    // Call getMaxRenderTargets() to force __maxRenderTargets to be set
    getMaxRenderTargets();

    // Create the render target array for the new frame buffer
    RenderTarget** renderTargets = new RenderTarget*[__maxRenderTargets];
    memset(renderTargets, 0, sizeof(RenderTarget*) * __maxRenderTargets);

    // Create the new frame buffer
    FrameBuffer* frameBuffer = new FrameBuffer(id ? id : "");
    frameBuffer->_handle = handle;
    frameBuffer->_renderTargets = renderTargets;

    // Add to the global list of managed frame buffers
    __frameBuffers.push_back(frameBuffer);

    return frameBuffer;
}

FrameBuffer* FrameBuffer::create(const char* id, unsigned int width, unsigned int height)
{
    // Create RenderTarget with same ID
    RenderTarget* renderTarget = RenderTarget::create(id, width, height);
    if (renderTarget == NULL)
    {
        return NULL;
    }

    // Create the frame buffer
    FrameBuffer* frameBuffer = create(id);
    if (frameBuffer == NULL)
    {
        return NULL;
    }

    // Add the render target as the first color attachment
    frameBuffer->setRenderTarget(renderTarget);
    SAFE_RELEASE(renderTarget);

    return frameBuffer;
}

FrameBuffer* FrameBuffer::getFrameBuffer(const char* id)
{
    // Search the vector for a matching ID.
    std::vector<FrameBuffer*>::const_iterator it;
    for (it = __frameBuffers.begin(); it < __frameBuffers.end(); it++)
    {
        FrameBuffer* fb = *it;
        if (strcmp(id, fb->getID()) == 0)
        {
            return fb;
        }
    }

    return NULL;
}

const char* FrameBuffer::getID() const
{
    return _id.c_str();
}

unsigned int FrameBuffer::getMaxRenderTargets()
{
    if (__maxRenderTargets == 0)
    {
#ifdef GL_MAX_COLOR_ATTACHMENTS
        GLint val;
        GL_ASSERT( glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &val) );
        __maxRenderTargets = (unsigned int) val;
#else
        __maxRenderTargets = 1;
#endif
    }

    return __maxRenderTargets;
}

void FrameBuffer::setRenderTarget(RenderTarget* target, unsigned int index)
{
    assert(index < __maxRenderTargets);

    if (_renderTargets[index] == target)
    {
        // No change
        return;
    }

    // Release our reference to the current RenderTarget at this index.
    SAFE_RELEASE(_renderTargets[index]);

    _renderTargets[index] = target;

    if (target)
    {
        // This FrameBuffer now references the RenderTarget.
        target->addRef();

        // Store the current FBO binding so we can restore it
        GLint currentFbo;
        GL_ASSERT( glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFbo) );

        // Now set this target as the color attachment corresponding to index.
        GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, _handle) );
        GLenum attachment = GL_COLOR_ATTACHMENT0 + index;
        GL_ASSERT( glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, _renderTargets[index]->getTexture()->getHandle(), 0) );

        // Restore the FBO binding
        GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, currentFbo) );
    }
}

RenderTarget* FrameBuffer::getRenderTarget(unsigned int index) const
{
    if (index < __maxRenderTargets)
    {
        return _renderTargets[index];
    }

    return NULL;
}

void FrameBuffer::setDepthStencilTarget(DepthStencilTarget* target)
{
    if (_depthStencilTarget == target)
    {
        return; // No change
    }

    // Release our existing depth stencil target
    SAFE_RELEASE(_depthStencilTarget);

    _depthStencilTarget = target;

    if (target)
    {
        // The FrameBuffer now owns this DepthStencilTarget
        target->addRef();

        // Store the current FBO binding so we can restore it
        GLint currentFbo;
        GL_ASSERT( glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFbo) );

        // Now set this target as the color attachment corresponding to index.
        GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, _handle) );

        // Bind the depth texture
        GL_ASSERT( glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthStencilTarget->getTexture()->getHandle(), 0) );

        // If the taget has a stencil buffer, bind that as well
        if (target->getFormat() == DepthStencilTarget::DEPTH24_STENCIL8)
        {
            GL_ASSERT( glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthStencilTarget->_stencilBuffer) );
        }

        // Restore the FBO binding
        GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, currentFbo) );
    }
}

DepthStencilTarget* FrameBuffer::getDepthStencilTarget() const
{
    return _depthStencilTarget;
}

void FrameBuffer::bind()
{
    // Bind this FrameBuffer for rendering.
    GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, _handle) );
}

void FrameBuffer::bindDefault()
{
    GL_ASSERT( glBindFramebuffer(GL_FRAMEBUFFER, 0) );
}

}
