#include <RHI/Vulkan/VKStd.h>

#include <RHI/Vulkan/VKCommands.h>
#include <RHI/Vulkan/VKDevice.h>
#include <RHI/Vulkan/VKRenderPass.h>

CCVKRenderPass::CCVKRenderPass()
{
  m_TypedID = GenerateObjectID<decltype(this)>();
}

CCVKRenderPass::~CCVKRenderPass()
{
  destroy();
}

void CCVKRenderPass::doInit(const RenderPassInfo& /*info*/)
{
  _gpuRenderPass = EZ_DEFAULT_NEW(CCVKGPURenderPass);
  _gpuRenderPass->colorAttachments = _colorAttachments;
  _gpuRenderPass->depthStencilAttachment = _depthStencilAttachment;
  _gpuRenderPass->subpasses = _subpasses;
  _gpuRenderPass->dependencies = _dependencies;

  // assign a dummy subpass if not specified
  if (_gpuRenderPass->subpasses.IsEmpty())
  {
    _gpuRenderPass->subpasses.ExpandAndGetRef();
    auto& subpass = _gpuRenderPass->subpasses.PeekBack();
    subpass.colors.SetCount(_colorAttachments.GetCount());
    for (ezUInt32 i = 0U; i < _colorAttachments.GetCount(); ++i)
    {
      subpass.colors[i] = i;
    }
    if (_depthStencilAttachment.format != Format::UNKNOWN)
    {
      subpass.depthStencil = _colorAttachments.GetCount();
    }
  }
  else
  {
    // unify depth stencil index
    ezUInt32 colorCount = _gpuRenderPass->colorAttachments.GetCount();
    for (auto& subpass : _gpuRenderPass->subpasses)
    {
      if (subpass.depthStencil != INVALID_BINDING && subpass.depthStencil > colorCount)
      {
        subpass.depthStencil = colorCount;
      }
      if (subpass.depthStencilResolve != INVALID_BINDING && subpass.depthStencilResolve > colorCount)
      {
        subpass.depthStencilResolve = colorCount;
      }
    }
  }

  cmdFuncCCVKCreateRenderPass(CCVKDevice::getInstance(), _gpuRenderPass);
}

void CCVKRenderPass::doDestroy()
{
  if (_gpuRenderPass)
  {
    CCVKDevice::getInstance()->gpuRecycleBin()->collect(_gpuRenderPass);
    _gpuRenderPass = nullptr;
  }
}
