#pragma once

#include <RHI/Base/Object.h>

class EZ_RHI_DLL Buffer : public RHIObject
{
public:
  Buffer();
  ~Buffer() override;

  static ezUInt32 computeHash(const BufferInfo& info);

  void initialize(const BufferInfo& info);
  void initialize(const BufferViewInfo& info);
  void resize(ezUInt32 size);
  void destroy();

  virtual void update(const void* buffer, ezUInt32 size) = 0;

  inline void update(const void* buffer) { update(buffer, _size); }

  inline BufferUsage getUsage() const { return _usage; }
  inline MemoryUsage getMemUsage() const { return _memUsage; }
  inline ezUInt32 getStride() const { return _stride; }
  inline ezUInt32 getCount() const { return _count; }
  inline ezUInt32 getSize() const { return _size; }
  inline BufferFlags getFlags() const { return _flags; }
  inline bool isBufferView() const { return _isBufferView; }

protected:
  virtual void doInit(const BufferInfo& info) = 0;
  virtual void doInit(const BufferViewInfo& info) = 0;
  virtual void doResize(ezUInt32 size, ezUInt32 count) = 0;
  virtual void doDestroy() = 0;

  BufferUsage _usage = BufferUsageBit::NONE;
  MemoryUsage _memUsage = MemoryUsageBit::NONE;
  ezUInt32 _stride = 0U;
  ezUInt32 _count = 0U;
  ezUInt32 _size = 0U;
  ezUInt32 _offset = 0U;
  BufferFlags _flags = BufferFlagBit::NONE;
  bool _isBufferView = false;
};
