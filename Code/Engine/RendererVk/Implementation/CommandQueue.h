#pragma once

#include <RendererVk/RendererVkDLL.h>
#include <vulkan/vulkan.hpp>

namespace ezInternal::Vk
{
  class CommandQueue : public ezRefCounted
  {
  public:
    CommandQueue(ezGALDeviceVk& device, CommandListType type, ezUInt32 queueFamilyIndex);
    void Wait(const ezGALFence* fence, ezUInt64 value) const;
    void Signal(const ezGALFence* fence, ezUInt64 value) const;
    void ExecuteCommandList(ezSharedPtr<CommandList> commandList);
    void ExecuteCommandLists(const ezDynamicArray<ezSharedPtr<CommandList>>& commandList);

    ezGALDeviceVk& GetDevice();
    ezUInt32 GetQueueFamilyIndex();
    vk::Queue GetQueue();

  private:
    ezGALDeviceVk& m_Device;
    ezUInt32 m_QueueFamilyIndex;
    vk::Queue m_Queue;
  };
} // namespace ezInternal::Vk
