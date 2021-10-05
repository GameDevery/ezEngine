#include <RHI/Vulkan/VKStd.h>

#include <RHI/Base/Def.h>
#include <RHI/Vulkan/VKCommandBuffer.h>
#include <RHI/Vulkan/VKCommands.h>
#include <RHI/Vulkan/VKDevice.h>
#include <RHI/Vulkan/VKSwapchain.h>
#include <RHI/Vulkan/VKTexture.h>

CCVKTexture::CCVKTexture()
{
  m_TypedID = GenerateObjectID<decltype(this)>();
}

CCVKTexture::~CCVKTexture()
{
  destroy();
}

void CCVKTexture::doInit(const TextureInfo& /*info*/)
{
  _gpuTexture = EZ_DEFAULT_NEW(CCVKGPUTexture);
  _gpuTexture->type = _info.type;
  _gpuTexture->format = _info.format;
  _gpuTexture->usage = _info.usage;
  _gpuTexture->width = _info.width;
  _gpuTexture->height = _info.height;
  _gpuTexture->depth = _info.depth;
  _gpuTexture->arrayLayers = _info.layerCount;
  _gpuTexture->mipLevels = _info.levelCount;
  _gpuTexture->samples = _info.samples;
  _gpuTexture->flags = _info.flags;
  _gpuTexture->size = _size;

  cmdFuncCCVKCreateTexture(CCVKDevice::getInstance(), _gpuTexture);

  if (!_gpuTexture->memoryless)
  {
    CCVKDevice::getInstance()->getMemoryStatus().textureSize += _size;
  }

  _gpuTextureView = EZ_DEFAULT_NEW(CCVKGPUTextureView);
  createTextureView();
}

void CCVKTexture::doInit(const TextureViewInfo& info)
{
  _gpuTexture = static_cast<CCVKTexture*>(info.texture)->gpuTexture();

  _gpuTextureView = EZ_DEFAULT_NEW(CCVKGPUTextureView);
  createTextureView();
}

void CCVKTexture::createTextureView()
{
  _gpuTextureView->gpuTexture = _gpuTexture;
  _gpuTextureView->type = _viewInfo.type;
  _gpuTextureView->format = _viewInfo.format;
  _gpuTextureView->baseLevel = _viewInfo.baseLevel;
  _gpuTextureView->levelCount = _viewInfo.levelCount;
  _gpuTextureView->baseLayer = _viewInfo.baseLayer;
  _gpuTextureView->layerCount = _viewInfo.layerCount;
  cmdFuncCCVKCreateTextureView(CCVKDevice::getInstance(), _gpuTextureView);
}

void CCVKTexture::doDestroy()
{
  if (_gpuTextureView)
  {
    CCVKDevice::getInstance()->gpuRecycleBin()->collect(_gpuTextureView);
    CCVKDevice::getInstance()->gpuDescriptorHub()->disengage(_gpuTextureView);
    EZ_DEFAULT_DELETE(_gpuTextureView);
    _gpuTextureView = nullptr;
  }

  if (_gpuTexture)
  {
    if (!_isTextureView)
    {
      if (!_gpuTexture->memoryless)
      {
        CCVKDevice::getInstance()->getMemoryStatus().textureSize -= _size;
      }
      CCVKDevice::getInstance()->gpuRecycleBin()->collect(_gpuTexture);
      CCVKDevice::getInstance()->gpuBarrierManager()->cancel(_gpuTexture);
      EZ_DEFAULT_DELETE(_gpuTexture);
    }
    _gpuTexture = nullptr;
  }
}

void CCVKTexture::doResize(ezUInt32 width, ezUInt32 height, ezUInt32 size)
{
  if (!width || !height)
    return;

  if (!_gpuTexture->memoryless)
  {
    CCVKDevice::getInstance()->getMemoryStatus().textureSize -= _size;
  }

  CCVKDevice::getInstance()->gpuRecycleBin()->collect(_gpuTextureView);
  CCVKDevice::getInstance()->gpuRecycleBin()->collect(_gpuTexture);

  _gpuTexture->width = width;
  _gpuTexture->height = height;
  _gpuTexture->size = size;
  cmdFuncCCVKCreateTexture(CCVKDevice::getInstance(), _gpuTexture);

  if (!_gpuTexture->memoryless)
  {
    CCVKDevice::getInstance()->getMemoryStatus().textureSize += size;
  }

  cmdFuncCCVKCreateTextureView(CCVKDevice::getInstance(), _gpuTextureView);
}

///////////////////////////// Swapchain Specific /////////////////////////////

void CCVKTexture::doInit(const SwapchainTextureInfo& /*info*/)
{
  _gpuTexture = EZ_DEFAULT_NEW(CCVKGPUTexture);
  _gpuTexture->type = _info.type;
  _gpuTexture->format = _info.format;
  _gpuTexture->usage = _info.usage;
  _gpuTexture->width = _info.width;
  _gpuTexture->height = _info.height;
  _gpuTexture->depth = _info.depth;
  _gpuTexture->arrayLayers = _info.layerCount;
  _gpuTexture->mipLevels = _info.levelCount;
  _gpuTexture->flags = _info.flags;
  _gpuTexture->samples = _info.samples;
  _gpuTexture->size = _size;

  _gpuTexture->swapchain = static_cast<CCVKSwapchain*>(_swapchain)->gpuSwapchain();
  _gpuTexture->memoryless = true;

  _gpuTextureView = EZ_DEFAULT_NEW(CCVKGPUTextureView);
  _gpuTextureView->gpuTexture = _gpuTexture;
  _gpuTextureView->type = _viewInfo.type;
  _gpuTextureView->format = _viewInfo.format;
  _gpuTextureView->baseLevel = _viewInfo.baseLevel;
  _gpuTextureView->levelCount = _viewInfo.levelCount;
  _gpuTextureView->baseLayer = _viewInfo.baseLayer;
  _gpuTextureView->layerCount = _viewInfo.layerCount;
}
