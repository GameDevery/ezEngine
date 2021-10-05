#include <RHI/Vulkan/VKStd.h>

#include <RHI/Vulkan/VKCommands.h>
#include <RHI/Vulkan/VKDevice.h>
#include <RHI/Vulkan/VKFramebuffer.h>
#include <RHI/Vulkan/VKRenderPass.h>
#include <RHI/Vulkan/VKTexture.h>

CCVKFramebuffer::CCVKFramebuffer()
{
  m_TypedID = GenerateObjectID<decltype(this)>();
}

CCVKFramebuffer::~CCVKFramebuffer()
{
  destroy();
}

void CCVKFramebuffer::doInit(const FramebufferInfo& /*info*/)
{
  _gpuFBO = EZ_DEFAULT_NEW(CCVKGPUFramebuffer);
  _gpuFBO->gpuRenderPass = static_cast<CCVKRenderPass*>(_renderPass)->gpuRenderPass();

  _gpuFBO->gpuColorViews.SetCount(_colorTextures.GetCount());
  for (ezUInt32 i = 0; i < _colorTextures.GetCount(); ++i)
  {
    auto* colorTex = static_cast<CCVKTexture*>(_colorTextures[i]);
    if (colorTex)
    {
      _gpuFBO->gpuColorViews[i] = colorTex->gpuTextureView();
    }
  }

  if (_depthStencilTexture)
  {
    _gpuFBO->gpuDepthStencilView = static_cast<CCVKTexture*>(_depthStencilTexture)->gpuTextureView();
  }

  cmdFuncCCVKCreateFramebuffer(CCVKDevice::getInstance(), _gpuFBO);
}

void CCVKFramebuffer::doDestroy()
{
  if (_gpuFBO)
  {
    CCVKDevice::getInstance()->gpuRecycleBin()->collect(_gpuFBO);
    _gpuFBO = nullptr;
  }
}
