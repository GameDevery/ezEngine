#include <RHI/Base/Swapchain.h>

Swapchain::Swapchain()
  : RHIObject(ObjectType::SWAPCHAIN)
{
}

Swapchain::~Swapchain() = default;

void Swapchain::initialize(const SwapchainInfo& info)
{
  EZ_ASSERT_DEV(info.windowHandle, "Invalid window handle");

  _windowHandle = info.windowHandle;
  _vsyncMode = info.vsyncMode;

  doInit(info);
}

void Swapchain::destroy()
{
  doDestroy();

  _windowHandle = nullptr;
}

void Swapchain::resize(ezUInt32 width, ezUInt32 height, SurfaceTransform transform)
{
  // if pre-rotation is enabled, width & height should always be measured in un-oriented space
  ezUInt32 newWidth = width;
  ezUInt32 newHeight = height;
  if (_preRotationEnabled && (ToNumber(transform) & 1))
    std::swap(newWidth, newHeight);

  if (newWidth != _colorTexture->getWidth() || newHeight != _colorTexture->getHeight() ||
      (_preRotationEnabled && transform != _transform))
  {
    doResize(width, height, transform); // pass oriented size

    _colorTexture->_info.width = _depthStencilTexture->_info.width = newWidth;
    _colorTexture->_info.height = _depthStencilTexture->_info.height = newHeight;
    if (_preRotationEnabled)
      _transform = transform; // only update transform when using pre-rotation
  }
}
