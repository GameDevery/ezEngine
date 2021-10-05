#include <RHI/Base/Texture.h>

Texture::Texture()
  : RHIObject(ObjectType::TEXTURE)
{
}

Texture::~Texture() = default;

ezUInt32 Texture::computeHash(const TextureInfo& info)
{
  ezUInt32 seed = 10;
  seed ^= ToNumber(info.type) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= ToNumber(info.usage) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= ToNumber(info.format) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= ToNumber(info.flags) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= ToNumber(info.samples) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= (info.width) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= (info.height) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= (info.depth) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= (info.layerCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= (info.levelCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  return seed;
}

ezUInt32 Texture::computeHash(const TextureViewInfo& info)
{
  ezUInt32 seed = 7;
  seed ^= info.texture->getHash() + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= ToNumber(info.type) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= ToNumber(info.format) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= (info.baseLevel) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= (info.levelCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= (info.baseLayer) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= (info.layerCount) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  return seed;
}

ezUInt32 Texture::computeHash(const Texture* texture)
{
  ezUInt32 hash = texture->isTextureView() ? computeHash(texture->getViewInfo()) : computeHash(texture->getInfo());
  if (texture->_swapchain)
    hash ^= (texture->_swapchain->GetObjectID()) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
  return hash;
}

void Texture::initialize(const TextureInfo& info)
{
  _info = info;
  _size = FormatSize(_info.format, _info.width, _info.height, _info.depth);
  _hash = computeHash(info);

  _viewInfo.texture = this;
  _viewInfo.format = _info.format;
  _viewInfo.type = _info.type;
  _viewInfo.baseLayer = 0;
  _viewInfo.layerCount = _info.layerCount;
  _viewInfo.baseLevel = 0;
  _viewInfo.levelCount = _info.levelCount;

  doInit(info);
}

void Texture::initialize(const TextureViewInfo& info)
{
  _info = info.texture->getInfo();
  _viewInfo = info;

  _isTextureView = true;
  _size = FormatSize(_info.format, _info.width, _info.height, _info.depth);
  _hash = computeHash(info);

  doInit(info);
}

void Texture::resize(ezUInt32 width, ezUInt32 height)
{
  if (_info.width != width || _info.height != height)
  {
    ezUInt32 size = FormatSize(_info.format, width, height, _info.depth);
    doResize(width, height, size);

    _info.width = width;
    _info.height = height;
    _size = size;
    _hash = computeHash(this);
  }
}

void Texture::destroy()
{
  doDestroy();

  _info = TextureInfo();
  _viewInfo = TextureViewInfo();

  _isTextureView = false;
  _size = _hash = 0;
}

///////////////////////////// Swapchain Specific /////////////////////////////

void Texture::initialize(const SwapchainTextureInfo& info, Texture* out)
{
  out->_info.type = TextureType::TEX2D;
  out->_info.format = info.format;
  out->_info.width = info.width;
  out->_info.height = info.height;
  out->_info.layerCount = 1;
  out->_info.levelCount = 1;
  out->_info.depth = 1;
  out->_info.samples = SampleCount::ONE;
  out->_info.flags = TextureFlagBit::NONE;
  out->_info.usage = GFX_FORMAT_INFOS[ToNumber(info.format)].hasDepth
                       ? TextureUsageBit::DEPTH_STENCIL_ATTACHMENT
                       : TextureUsageBit::COLOR_ATTACHMENT;

  out->_swapchain = info.swapchain;
  out->_size = FormatSize(info.format, info.width, info.height, 1);
  out->_hash = computeHash(out);

  out->_viewInfo.texture = out;
  out->_viewInfo.format = out->_info.format;
  out->_viewInfo.type = out->_info.type;
  out->_viewInfo.baseLayer = 0;
  out->_viewInfo.layerCount = out->_info.layerCount;
  out->_viewInfo.baseLevel = 0;
  out->_viewInfo.levelCount = out->_info.levelCount;

  out->doInit(info);
}
