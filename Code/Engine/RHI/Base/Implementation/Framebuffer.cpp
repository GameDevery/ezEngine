#include <RHI/Base/Framebuffer.h>
#include <RHI/Base/Texture.h>

Framebuffer::Framebuffer()
  : RHIObject(ObjectType::FRAMEBUFFER)
{
}

Framebuffer::~Framebuffer() = default;

ezUInt32 Framebuffer::computeHash(const FramebufferInfo& info)
{
  auto seed = (info.colorTextures.GetCount() * 2 + (info.depthStencilTexture ? 2 : 0));
  for (const Texture* attachment : info.colorTextures)
  {
    seed ^= attachment->getHash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= attachment->GetTypedID() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
  if (info.depthStencilTexture)
  {
    seed ^= info.depthStencilTexture->getHash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= info.depthStencilTexture->GetTypedID() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
  return seed;
}

void Framebuffer::initialize(const FramebufferInfo& info)
{
  _renderPass = info.renderPass;
  _colorTextures = info.colorTextures;
  _depthStencilTexture = info.depthStencilTexture;

  doInit(info);
}

void Framebuffer::destroy()
{
  doDestroy();

  _renderPass = nullptr;
  _colorTextures.Clear();
  _depthStencilTexture = nullptr;
}
