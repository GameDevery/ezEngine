#pragma once

#include <RHI/Base/Queue.h>
#include <RHI/Vulkan/VKStd.h>

class CCVKGPUQueue;

class EZ_VULKAN_API CCVKQueue final : public Queue
{
public:
  CCVKQueue();
  ~CCVKQueue() override;

  void submit(CommandBuffer* const* cmdBuffs, ezUInt32 count) override;

  inline CCVKGPUQueue* gpuQueue() const { return _gpuQueue; }

protected:
  friend class CCVKDevice;

  void doInit(const QueueInfo& info) override;
  void doDestroy() override;

  CCVKGPUQueue* _gpuQueue = nullptr;

  ezUInt32 _numDrawCalls = 0;
  ezUInt32 _numInstances = 0;
  ezUInt32 _numTriangles = 0;
};
