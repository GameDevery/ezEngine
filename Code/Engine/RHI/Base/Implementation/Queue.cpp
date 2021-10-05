#include <RHI/Base/Queue.h>

Queue::Queue()
  : RHIObject(ObjectType::QUEUE)
{
}

Queue::~Queue() = default;

void Queue::initialize(const QueueInfo& info)
{
  _type = info.type;

  doInit(info);
}

void Queue::destroy()
{
  doDestroy();

  _type = QueueType::GRAPHICS;
}
