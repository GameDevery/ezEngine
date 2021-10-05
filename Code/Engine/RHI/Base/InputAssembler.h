#pragma once

#include <RHI/Base/Object.h>

class EZ_RHI_DLL InputAssembler : public RHIObject
{
public:
  InputAssembler();
  ~InputAssembler() override;

  void initialize(const InputAssemblerInfo& info);
  void destroy();

  inline const AttributeList& getAttributes() const { return _attributes; }
  inline const BufferList& getVertexBuffers() const { return _vertexBuffers; }
  inline Buffer* getIndexBuffer() const { return _indexBuffer; }
  inline Buffer* getIndirectBuffer() const { return _indirectBuffer; }
  inline ezUInt32 getAttributesHash() const { return _attributesHash; }

  inline const DrawInfo& getDrawInfo() const { return _drawInfo; }

  inline void setVertexCount(ezUInt32 count) { _drawInfo.vertexCount = count; }
  inline void setFirstVertex(ezUInt32 first) { _drawInfo.firstVertex = first; }
  inline void setIndexCount(ezUInt32 count) { _drawInfo.indexCount = count; }
  inline void setFirstIndex(ezUInt32 first) { _drawInfo.firstIndex = first; }
  inline void setVertexOffset(int32_t offset) { _drawInfo.vertexOffset = offset; }
  inline void setInstanceCount(ezUInt32 count) { _drawInfo.instanceCount = count; }
  inline void setFirstInstance(ezUInt32 first) { _drawInfo.firstInstance = first; }

  inline ezUInt32 getVertexCount() const { return _drawInfo.vertexCount; }
  inline ezUInt32 getFirstVertex() const { return _drawInfo.firstVertex; }
  inline ezUInt32 getIndexCount() const { return _drawInfo.indexCount; }
  inline ezUInt32 getFirstIndex() const { return _drawInfo.firstIndex; }
  inline ezUInt32 getVertexOffset() const { return _drawInfo.vertexOffset; }
  inline ezUInt32 getInstanceCount() const { return _drawInfo.instanceCount; }
  inline ezUInt32 getFirstInstance() const { return _drawInfo.firstInstance; }

protected:
  virtual void doInit(const InputAssemblerInfo& info) = 0;
  virtual void doDestroy() = 0;

  ezUInt32 computeAttributesHash() const;

  AttributeList _attributes;
  ezUInt32 _attributesHash = 0;

  BufferList _vertexBuffers;
  Buffer* _indexBuffer{nullptr};
  Buffer* _indirectBuffer{nullptr};

  DrawInfo _drawInfo;
};
