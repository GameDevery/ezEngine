#include <RHI/Base/Buffer.h>

Buffer::Buffer()
  : RHIObject(ObjectType::BUFFER)
{
}

Buffer::~Buffer() = default;

ezUInt32 Buffer::computeHash(const BufferInfo& info)
{
  ezUInt32 seed = 4;
  seed ^= static_cast<ezUInt32>(info.usage) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= static_cast<ezUInt32>(info.memUsage) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= static_cast<ezUInt32>(info.size) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  seed ^= static_cast<ezUInt32>(info.flags) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  return seed;
}

void Buffer::initialize(const BufferInfo& info)
{
  _usage = info.usage;
  _memUsage = info.memUsage;
  _size = info.size;
  _flags = info.flags;
  _stride = ezMath::Max(info.stride, 1U);
  _count = _size / _stride;

  doInit(info);
}

void Buffer::initialize(const BufferViewInfo& info)
{
  _usage = info.buffer->getUsage();
  _memUsage = info.buffer->getMemUsage();
  _flags = info.buffer->getFlags();
  _offset = info.offset;
  _size = _stride = info.range;
  _count = 1U;
  _isBufferView = true;

  doInit(info);
}

void Buffer::destroy()
{
  doDestroy();

  _offset = _size = _stride = _count = 0U;
}

void Buffer::resize(ezUInt32 size)
{
  if (size != _size)
  {
    ezUInt32 count = size / _stride;
    doResize(size, count);

    _size = size;
    _count = count;
  }
}
