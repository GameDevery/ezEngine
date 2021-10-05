#include <RHI/Base/Device.h>

Device* Device::instance = nullptr;

Device* Device::getInstance()
{
  return Device::instance;
}

Device::Device()
{
  Device::instance = this;
  //_features.fill(false);
  for (ezUInt32 i = 0; i < _features.GetCount(); i++)
  {
    _features[i] = false;
  }
}

Device::~Device()
{
  Device::instance = nullptr;
}

bool Device::initialize(const DeviceInfo& info)
{
  _bindingMappingInfo = info.bindingMappingInfo;
  if (_bindingMappingInfo.bufferOffsets.IsEmpty())
  {
    _bindingMappingInfo.bufferOffsets.PushBack(0);
  }
  if (_bindingMappingInfo.samplerOffsets.IsEmpty())
  {
    _bindingMappingInfo.samplerOffsets.PushBack(0);
  }

  return doInit(info);
}

void Device::destroy()
{
  for (auto pair : _samplers)
  {
    EZ_DEFAULT_DELETE(pair.value);
  }

  for (auto pair : _globalBarriers)
  {
    EZ_DEFAULT_DELETE(pair.value);
  }

  for (auto pair : _textureBarriers)
  {
    EZ_DEFAULT_DELETE(pair.value);
  }

  _bindingMappingInfo.bufferOffsets.Clear();
  _bindingMappingInfo.samplerOffsets.Clear();

  doDestroy();
}

void Device::destroySurface(void* windowHandle)
{
  for (auto* swapchain : _swapchains)
  {
    if (swapchain->getWindowHandle() == windowHandle)
    {
      swapchain->destroySurface();
      break;
    }
  }
}

void Device::createSurface(void* windowHandle)
{
  for (auto* swapchain : _swapchains)
  {
    if (!swapchain->getWindowHandle())
    {
      swapchain->createSurface(windowHandle);
      break;
    }
  }
}
