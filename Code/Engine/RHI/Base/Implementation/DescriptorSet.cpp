#include <RHI/Base/DescriptorSet.h>
#include <RHI/Base/DescriptorSetLayout.h>

DescriptorSet::DescriptorSet()
  : RHIObject(ObjectType::DESCRIPTOR_SET)
{
}

DescriptorSet::~DescriptorSet() = default;

void DescriptorSet::initialize(const DescriptorSetInfo& info)
{
  EZ_ASSERT_DEV(info.layout, "Invalid set layout");

  _layout = info.layout;
  ezUInt32 descriptorCount = _layout->getDescriptorCount();
  _buffers.SetCount(descriptorCount);
  _textures.SetCount(descriptorCount);
  _samplers.SetCount(descriptorCount);

  doInit(info);
}

void DescriptorSet::destroy()
{
  doDestroy();

  _layout = nullptr;
  // have to clear these or else it might not be properly updated when reused
  _buffers.Clear();
  _textures.Clear();
  _samplers.Clear();
}

void DescriptorSet::bindBuffer(ezUInt32 binding, Buffer* buffer, ezUInt32 index)
{
  const ezUInt32 descriptorIndex = _layout->getDescriptorIndices()[binding];
  if (_buffers[descriptorIndex + index] != buffer)
  {
    _buffers[descriptorIndex + index] = buffer;
    _isDirty = true;
  }
}

void DescriptorSet::bindTexture(ezUInt32 binding, Texture* texture, ezUInt32 index)
{
  const ezUInt32 descriptorIndex = _layout->getDescriptorIndices()[binding];
  if (_textures[descriptorIndex + index] != texture)
  {
    _textures[descriptorIndex + index] = texture;
    _isDirty = true;
  }
}

void DescriptorSet::bindSampler(ezUInt32 binding, Sampler* sampler, ezUInt32 index)
{
  const ezUInt32 descriptorIndex = _layout->getDescriptorIndices()[binding];
  if (_samplers[descriptorIndex + index] != sampler)
  {
    _samplers[descriptorIndex + index] = sampler;
    _isDirty = true;
  }
}

bool DescriptorSet::bindBufferJSB(ezUInt32 binding, Buffer* buffer, ezUInt32 index)
{
  bindBuffer(binding, buffer, index);
  return _isDirty;
}

bool DescriptorSet::bindTextureJSB(ezUInt32 binding, Texture* texture, ezUInt32 index)
{
  bindTexture(binding, texture, index);
  return _isDirty;
}

bool DescriptorSet::bindSamplerJSB(ezUInt32 binding, Sampler* sampler, ezUInt32 index)
{
  bindSampler(binding, sampler, index);
  return _isDirty;
}

Buffer* DescriptorSet::getBuffer(ezUInt32 binding, ezUInt32 index) const
{
  const ezDynamicArray<ezUInt32>& descriptorIndices = _layout->getDescriptorIndices();
  if (binding >= descriptorIndices.GetCount())
    return nullptr;
  const ezUInt32 descriptorIndex = descriptorIndices[binding] + index;
  if (descriptorIndex >= _buffers.GetCount())
    return nullptr;
  return _buffers[descriptorIndex];
}

Texture* DescriptorSet::getTexture(ezUInt32 binding, ezUInt32 index) const
{
  const ezDynamicArray<ezUInt32>& descriptorIndices = _layout->getDescriptorIndices();
  if (binding >= descriptorIndices.GetCount())
    return nullptr;
  const ezUInt32 descriptorIndex = descriptorIndices[binding] + index;
  if (descriptorIndex >= _textures.GetCount())
    return nullptr;
  return _textures[descriptorIndex];
}

Sampler* DescriptorSet::getSampler(ezUInt32 binding, ezUInt32 index) const
{
  const ezDynamicArray<ezUInt32>& descriptorIndices = _layout->getDescriptorIndices();
  if (binding >= descriptorIndices.GetCount())
    return nullptr;
  const ezUInt32 descriptorIndex = descriptorIndices[binding] + index;
  if (descriptorIndex >= _samplers.GetCount())
    return nullptr;
  return _samplers[descriptorIndex];
}
