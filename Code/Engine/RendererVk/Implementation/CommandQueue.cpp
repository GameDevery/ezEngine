#include <RendererVkPCH.h>

#include <RendererVk/Implementation/CommandQueue.h>
#include <RendererVk/Implementation/CommandList.h>
#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/Resources/FenceVk.h>

namespace ezInternal::Vk
{
  CommandQueue::CommandQueue(ezGALDeviceVk& device, CommandListType type, ezUInt32 queueFamilyIndex)
    : m_Device{device}
    , m_QueueFamilyIndex{queueFamilyIndex}
  {
    m_Queue = m_Device.GetVkDevice().getQueue(m_QueueFamilyIndex, 0);
  }

  void CommandQueue::Wait(const ezGALFence* fence, ezUInt64 value) const
  {
    const ezGALFenceVk* vkFence = static_cast<const ezGALFenceVk*>(fence);

    vk::TimelineSemaphoreSubmitInfo timelineInfo = {};
    timelineInfo.waitSemaphoreValueCount = 1;
    timelineInfo.pWaitSemaphoreValues = &value;

    vk::SubmitInfo signalSubmitInfo = {};
    signalSubmitInfo.pNext = &timelineInfo;
    signalSubmitInfo.waitSemaphoreCount = 1;
    signalSubmitInfo.pWaitSemaphores = &vkFence->GetVkFence();
    vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eAllCommands;
    signalSubmitInfo.pWaitDstStageMask = &waitDstStageMask;
    vk::Result result = m_Queue.submit(1, &signalSubmitInfo, {});
  }

  void CommandQueue::Signal(const ezGALFence* fence, ezUInt64 value) const
  {
    const ezGALFenceVk* vkFence = static_cast<const ezGALFenceVk*>(fence);

    vk::TimelineSemaphoreSubmitInfo timelineInfo = {};
    timelineInfo.signalSemaphoreValueCount = 1;
    timelineInfo.pSignalSemaphoreValues = &value;

    vk::SubmitInfo signalSubmitInfo = {};
    signalSubmitInfo.pNext = &timelineInfo;
    signalSubmitInfo.signalSemaphoreCount = 1;
    signalSubmitInfo.pSignalSemaphores = &vkFence->GetVkFence();
    vk::Result result = m_Queue.submit(1, &signalSubmitInfo, {});
  }

  void CommandQueue::ExecuteCommandList(ezSharedPtr<CommandList> commandList)
  {
    ezDynamicArray<ezSharedPtr<CommandList>> commandLists;
    commandLists.PushBack(commandList);
    ExecuteCommandLists(commandLists);
  }

  void CommandQueue::ExecuteCommandLists(const ezDynamicArray<ezSharedPtr<CommandList>>& commandLists)
  {
    ezDynamicArray<vk::CommandBuffer> vkCommandBuffers;
    for (auto& commandList : commandLists)
    {
      if (!commandList)
        continue;
      vkCommandBuffers.PushBack(commandList->GetCommandBuffer());
    }

    vk::SubmitInfo submitInfo = {};
    submitInfo.commandBufferCount = vkCommandBuffers.GetCount();
    submitInfo.pCommandBuffers = vkCommandBuffers.GetData();

    vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eAllCommands;
    submitInfo.pWaitDstStageMask = &waitDstStageMask;

    vk::Result result = m_Queue.submit(1, &submitInfo, {});
  }

  ezGALDeviceVk& CommandQueue::GetDevice()
  {
    return m_Device;
  }

  ezUInt32 CommandQueue::GetQueueFamilyIndex()
  {
    return m_QueueFamilyIndex;
  }

  vk::Queue CommandQueue::GetQueue()
  {
    return m_Queue;
  }

} // namespace ezInternal::Vk
