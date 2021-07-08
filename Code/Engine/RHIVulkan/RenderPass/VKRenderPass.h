#pragma once
#include <RHI/RenderPass/RenderPass.h>
#include <RHIVulkan/RHIVulkanDLL.h>

class VKDevice;

class VKRenderPass : public RenderPass
{
public:
    VKRenderPass(VKDevice& device, const RenderPassDesc& desc);
    const RenderPassDesc& GetDesc() const override;

    vk::RenderPass GetRenderPass() const;

private:
    RenderPassDesc m_desc;
    vk::UniqueRenderPass m_render_pass;
};
