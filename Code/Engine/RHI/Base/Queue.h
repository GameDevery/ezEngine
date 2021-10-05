#pragma once

#include <RHI/Base/Object.h>

class EZ_RHI_DLL Queue : public RHIObject
{
public:
  Queue();
  ~Queue() override;

  void initialize(const QueueInfo& info);
  void destroy();

  virtual void submit(CommandBuffer* const* cmdBuffs, ezUInt32 count) = 0;

  inline void submit(const CommandBufferList& cmdBuffs) { submit(cmdBuffs.GetData(), cmdBuffs.GetCount()); }

  inline QueueType getType() const { return _type; }

protected:
  virtual void doInit(const QueueInfo& info) = 0;
  virtual void doDestroy() = 0;

  QueueType _type = QueueType::GRAPHICS;
};
