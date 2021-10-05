#include <RHI/Base/InputAssembler.h>

InputAssembler::InputAssembler()
  : RHIObject(ObjectType::INPUT_ASSEMBLER)
{
}

InputAssembler::~InputAssembler() = default;

ezUInt32 InputAssembler::computeAttributesHash() const
{
  // https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
  // 6: Attribute has 6 elements.
  ezUInt32 seed = _attributes.GetCount() * 6;
  for (const auto& attribute : _attributes)
  {
    //seed ^= std::hash<ezString>{}(attribute.name) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    //seed ^= static_cast<ezUInt32>(attribute.format) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    //seed ^= attribute.isNormalized + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    //seed ^= attribute.stream + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    //seed ^= attribute.isInstanced + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    //seed ^= attribute.location + 0x9e3779b9 + (seed << 6) + (seed >> 2);

    seed = ezHashingUtils::CombineHashValues32(seed, ezHashingUtils::StringHashTo32(ezHashingUtils::StringHash(attribute.name)));
    seed = ezHashingUtils::CombineHashValues32(seed, static_cast<ezUInt32>(attribute.format));
    seed = ezHashingUtils::CombineHashValues32(seed, attribute.isNormalized);
    seed = ezHashingUtils::CombineHashValues32(seed, attribute.stream);
    seed = ezHashingUtils::CombineHashValues32(seed, attribute.isInstanced);
    seed = ezHashingUtils::CombineHashValues32(seed, attribute.location);
  }
  return static_cast<ezUInt32>(seed);
}

void InputAssembler::initialize(const InputAssemblerInfo& info)
{
  _attributes = info.attributes;
  _vertexBuffers = info.vertexBuffers;
  _indexBuffer = info.indexBuffer;
  _indirectBuffer = info.indirectBuffer;
  _attributesHash = computeAttributesHash();

  if (_indexBuffer)
  {
    _drawInfo.indexCount = _indexBuffer->getCount();
    _drawInfo.firstIndex = 0;
  }
  else if (!_vertexBuffers.IsEmpty())
  {
    _drawInfo.vertexCount = _vertexBuffers[0]->getCount();
    _drawInfo.firstVertex = 0;
    _drawInfo.vertexOffset = 0;
  }

  doInit(info);
}

void InputAssembler::destroy()
{
  doDestroy();

  _attributes.Clear();
  _attributesHash = 0U;

  _vertexBuffers.Clear();
  _indexBuffer = nullptr;
  _indirectBuffer = nullptr;

  _drawInfo = DrawInfo();
}
