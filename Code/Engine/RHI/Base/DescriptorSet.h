#pragma once

#include <RHI/Base/Object.h>

class EZ_RHI_DLL DescriptorSet : public RHIObject
{
public:
  DescriptorSet();
  ~DescriptorSet() override;

  void initialize(const DescriptorSetInfo& info);
  void destroy();

  virtual void update() = 0;

  virtual void bindBuffer(ezUInt32 binding, Buffer* buffer, ezUInt32 index);
  virtual void bindTexture(ezUInt32 binding, Texture* texture, ezUInt32 index);
  virtual void bindSampler(ezUInt32 binding, Sampler* sampler, ezUInt32 index);

  // Functions invoked by JSB adapter
  bool bindBufferJSB(ezUInt32 binding, Buffer* buffer, ezUInt32 index);
  bool bindTextureJSB(ezUInt32 binding, Texture* texture, ezUInt32 index);
  bool bindSamplerJSB(ezUInt32 binding, Sampler* sampler, ezUInt32 index);

  Buffer* getBuffer(ezUInt32 binding, ezUInt32 index) const;
  Texture* getTexture(ezUInt32 binding, ezUInt32 index) const;
  Sampler* getSampler(ezUInt32 binding, ezUInt32 index) const;

  inline DescriptorSetLayout* getLayout() { return _layout; }

  inline void bindBuffer(ezUInt32 binding, Buffer* buffer) { bindBuffer(binding, buffer, 0U); }
  inline void bindTexture(ezUInt32 binding, Texture* texture) { bindTexture(binding, texture, 0U); }
  inline void bindSampler(ezUInt32 binding, Sampler* sampler) { bindSampler(binding, sampler, 0U); }
  inline Buffer* getBuffer(ezUInt32 binding) const { return getBuffer(binding, 0U); }
  inline Texture* getTexture(ezUInt32 binding) const { return getTexture(binding, 0U); }
  inline Sampler* getSampler(ezUInt32 binding) const { return getSampler(binding, 0U); }

protected:
  virtual void doInit(const DescriptorSetInfo& info) = 0;
  virtual void doDestroy() = 0;

  DescriptorSetLayout* _layout = nullptr;
  BufferList _buffers;
  TextureList _textures;
  SamplerList _samplers;

  bool _isDirty = false;
};
