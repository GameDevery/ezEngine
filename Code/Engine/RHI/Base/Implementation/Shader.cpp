#include <RHI/Base/Shader.h>

Shader::Shader()
  : RHIObject(ObjectType::SHADER)
{
}

Shader::~Shader() = default;

void Shader::initialize(const ShaderInfo& info)
{
  _name = info.name;
  _stages = info.stages;
  _attributes = info.attributes;
  _blocks = info.blocks;
  _buffers = info.buffers;
  _samplerTextures = info.samplerTextures;
  _samplers = info.samplers;
  _textures = info.textures;
  _images = info.images;
  _subpassInputs = info.subpassInputs;

  doInit(info);
}

void Shader::destroy()
{
  doDestroy();

  _stages.Clear();
  _attributes.Clear();
  _blocks.Clear();
  _buffers.Clear();
  _samplerTextures.Clear();
  _samplers.Clear();
  _textures.Clear();
  _images.Clear();
  _subpassInputs.Clear();
}
