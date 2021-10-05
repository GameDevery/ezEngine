#pragma once

#include <RHI/Base/Object.h>

class EZ_RHI_DLL Texture : public RHIObject
{
public:
  Texture();
  ~Texture() override;

  static ezUInt32 computeHash(const TextureInfo& info);
  static ezUInt32 computeHash(const TextureViewInfo& info);

  void initialize(const TextureInfo& info);
  void initialize(const TextureViewInfo& info);
  void resize(ezUInt32 width, ezUInt32 height);
  void destroy();

  inline const TextureInfo& getInfo() const { return _info; }
  inline const TextureViewInfo& getViewInfo() const { return _viewInfo; }

  inline bool isTextureView() const { return _isTextureView; }
  inline ezUInt32 getSize() const { return _size; }
  inline ezUInt32 getHash() const { return _hash; }

  // convenient getter for common usages
  inline Format getFormat() const { return _info.format; }
  inline ezUInt32 getWidth() const { return _info.width; }
  inline ezUInt32 getHeight() const { return _info.height; }

protected:
  friend class Swapchain;

  virtual void doInit(const TextureInfo& info) = 0;
  virtual void doInit(const TextureViewInfo& info) = 0;
  virtual void doDestroy() = 0;
  virtual void doResize(ezUInt32 width, ezUInt32 height, ezUInt32 size) = 0;

  static ezUInt32 computeHash(const Texture* texture);
  static void initialize(const SwapchainTextureInfo& info, Texture* out);
  virtual void doInit(const SwapchainTextureInfo& info) = 0;

  TextureInfo _info;
  TextureViewInfo _viewInfo;

  Swapchain* _swapchain{nullptr};
  bool _isTextureView{false};
  ezUInt32 _size{0U};
  ezUInt32 _hash{0U};
};
