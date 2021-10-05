#pragma once

#include <RHI/Base/Object.h>
#include <RHI/Base/Texture.h>

class EZ_RHI_DLL Swapchain : public RHIObject
{
public:
  Swapchain();
  ~Swapchain() override;

  void initialize(const SwapchainInfo& info);
  void destroy();

  /**
     * Resize the swapchain with the given metric.
     * Note that you should invoke this function iff when there is actual
     * size or orientation changes, with the up-to-date information about
     * the underlying surface.
     *
     * @param width The width of the surface in oriented screen space
     * @param height The height of the surface in oriented screen space
     * @param transform The orientation of the surface
     */
  void resize(ezUInt32 width, ezUInt32 height, SurfaceTransform transform);

  inline void destroySurface();
  inline void createSurface(void* windowHandle);

  inline void* getWindowHandle() const { return _windowHandle; }
  inline VsyncMode getVSyncMode() const { return _vsyncMode; }

  inline Texture* getColorTexture() const { return _colorTexture; }
  inline Texture* getDepthStencilTexture() const { return _depthStencilTexture; }

  inline SurfaceTransform getSurfaceTransform() const { return _transform; }
  inline ezUInt32 getWidth() const { return _colorTexture->getWidth(); }
  inline ezUInt32 getHeight() const { return _colorTexture->getHeight(); }

protected:
  virtual void doInit(const SwapchainInfo& info) = 0;
  virtual void doDestroy() = 0;
  virtual void doResize(ezUInt32 width, ezUInt32 height, SurfaceTransform transform) = 0;
  virtual void doDestroySurface() = 0;
  virtual void doCreateSurface(void* windowHandle) = 0;

  static inline void initTexture(const SwapchainTextureInfo& info, Texture* texture);

  void* _windowHandle{nullptr};
  VsyncMode _vsyncMode{VsyncMode::RELAXED};
  SurfaceTransform _transform{SurfaceTransform::IDENTITY};
  bool _preRotationEnabled{false};

  Texture* _colorTexture{nullptr};
  Texture* _depthStencilTexture{nullptr};
};

///////////////////////////////////////////////////////////

void Swapchain::destroySurface()
{
  doDestroySurface();
  _windowHandle = nullptr;
}

void Swapchain::createSurface(void* windowHandle)
{
  _windowHandle = windowHandle;
  doCreateSurface(windowHandle);
}

void Swapchain::initTexture(const SwapchainTextureInfo& info, Texture* texture)
{
  Texture::initialize(info, texture);
}
