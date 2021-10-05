#pragma once

#include <RHI/Base/Object.h>

class EZ_RHI_DLL Shader : public RHIObject
{
public:
  Shader();
  ~Shader() override;

  void initialize(const ShaderInfo& info);
  void destroy();

  inline const ezString& getName() const { return _name; }
  inline const ShaderStageList& getStages() const { return _stages; }
  inline const AttributeList& getAttributes() const { return _attributes; }
  inline const UniformBlockList& getBlocks() const { return _blocks; }
  inline const UniformStorageBufferList& getBuffers() const { return _buffers; }
  inline const UniformSamplerTextureList& getSamplerTextures() const { return _samplerTextures; }
  inline const UniformSamplerList& getSamplers() const { return _samplers; }
  inline const UniformTextureList& getTextures() const { return _textures; }
  inline const UniformStorageImageList& getImages() const { return _images; }
  inline const UniformInputAttachmentList& getSubpassInputs() const { return _subpassInputs; }

protected:
  virtual void doInit(const ShaderInfo& info) = 0;
  virtual void doDestroy() = 0;

  ezString _name;
  ShaderStageList _stages;
  AttributeList _attributes;
  UniformBlockList _blocks;
  UniformStorageBufferList _buffers;
  UniformSamplerTextureList _samplerTextures;
  UniformSamplerList _samplers;
  UniformTextureList _textures;
  UniformStorageImageList _images;
  UniformInputAttachmentList _subpassInputs;
};
