#include <RHI/Base/CommandBuffer.h>

CommandBuffer::CommandBuffer()
  : RHIObject(ObjectType::COMMAND_BUFFER)
{
}

CommandBuffer::~CommandBuffer() = default;

void CommandBuffer::initialize(const CommandBufferInfo& info)
{
  _type = info.type;
  _queue = info.queue;

  doInit(info);
}

void CommandBuffer::destroy()
{
  doDestroy();

  _type = CommandBufferType::PRIMARY;
  _queue = nullptr;
}
