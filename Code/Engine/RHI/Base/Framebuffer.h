#pragma once

#include <RHI/Base/Object.h>

class EZ_RHI_DLL Framebuffer : public RHIObject
{
public:
  Framebuffer();
  ~Framebuffer() override;

  static ezUInt32 computeHash(const FramebufferInfo& info);

  void initialize(const FramebufferInfo& info);
  void destroy();

  inline RenderPass* getRenderPass() const { return _renderPass; }
  inline const TextureList& getColorTextures() const { return _colorTextures; }
  inline Texture* getDepthStencilTexture() const { return _depthStencilTexture; }

protected:
  virtual void doInit(const FramebufferInfo& info) = 0;
  virtual void doDestroy() = 0;

  RenderPass* _renderPass = nullptr;
  TextureList _colorTextures;
  Texture* _depthStencilTexture = nullptr;
};
