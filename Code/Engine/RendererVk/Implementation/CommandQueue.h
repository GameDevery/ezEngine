#pragma once

#include <RendererVk/RendererVkDLL.h>
#include <vulkan/vulkan.hpp>

namespace ezInternal::Vk
{
  class CommandQueue
  {
  public:
    CommandQueue(ezGALDeviceVk& device, CommandListType type, ezUInt32 queueFamilyIndex);
    void Wait(ezGALFence* fence, ezUInt64 value);
    void Signal(ezGALFence* fence, ezUInt64 value);
    void ExecuteCommandList(CommandList* commandList);
    void ExecuteCommandLists(const ezDynamicArray<CommandList*>& commandList);

    ezGALDeviceVk& GetDevice();
    ezUInt32 GetQueueFamilyIndex();
    vk::Queue GetQueue();

  private:
    ezGALDeviceVk& m_Device;
    ezUInt32 m_QueueFamilyIndex;
    vk::Queue m_Queue;
  };
} // namespace ezInternal::Vk
