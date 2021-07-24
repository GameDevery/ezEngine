#include <RHI/Framebuffer/FramebufferBase.h>

#include <RHI/RenderPass/RenderPass.h>

FramebufferBase::FramebufferBase(const FramebufferDesc& desc)
    : m_desc(desc)
{
}

const FramebufferDesc& FramebufferBase::GetDesc() const
{
    return m_desc;
}
