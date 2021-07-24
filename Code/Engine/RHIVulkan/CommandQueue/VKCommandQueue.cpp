#include <RHIVulkanPCH.h>

#include <RHIVulkan/CommandList/VKCommandList.h>
#include <RHIVulkan/CommandQueue/VKCommandQueue.h>
#include <RHIVulkan/Device/VKDevice.h>
#include <RHIVulkan/Fence/VKTimelineSemaphore.h>

VKCommandQueue::VKCommandQueue(VKDevice& device, CommandListType type, ezUInt32 queueFamilyIndex)
  : m_Device(device)
  , m_QueueFamilyIndex(queueFamilyIndex)
{
  m_Queue = m_Device.GetDevice().getQueue(m_QueueFamilyIndex, 0);
}

void VKCommandQueue::Wait(const ezSharedPtr<Fence>& fence, ezUInt64 value)
{
  ezSharedPtr<VKTimelineSemaphore> vkFence = fence.Downcast<VKTimelineSemaphore>();
  vk::TimelineSemaphoreSubmitInfo timelineInfo = {};
  timelineInfo.waitSemaphoreValueCount = 1;
  timelineInfo.pWaitSemaphoreValues = &value;

  vk::SubmitInfo signalSubmitInfo = {};
  signalSubmitInfo.pNext = &timelineInfo;
  signalSubmitInfo.waitSemaphoreCount = 1;
  signalSubmitInfo.pWaitSemaphores = &vkFence->GetFence();
  vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eAllCommands;
  signalSubmitInfo.pWaitDstStageMask = &waitDstStageMask;
  vk::Result res = m_Queue.submit(1, &signalSubmitInfo, {});
}

void VKCommandQueue::Signal(const ezSharedPtr<Fence>& fence, ezUInt64 value)
{
  ezSharedPtr<VKTimelineSemaphore> vkFence = fence.Downcast<VKTimelineSemaphore>();
  vk::TimelineSemaphoreSubmitInfo timelineInfo = {};
  timelineInfo.signalSemaphoreValueCount = 1;
  timelineInfo.pSignalSemaphoreValues = &value;

  vk::SubmitInfo signalSubmitInfo = {};
  signalSubmitInfo.pNext = &timelineInfo;
  signalSubmitInfo.signalSemaphoreCount = 1;
  signalSubmitInfo.pSignalSemaphores = &vkFence->GetFence();
  vk::Result result = m_Queue.submit(1, &signalSubmitInfo, {});
}

void VKCommandQueue::ExecuteCommandLists(const std::vector<ezSharedPtr<CommandList>>& commandLists)
{
  ezDynamicArray<vk::CommandBuffer> vkCommandLists;
  for (auto& commandList : commandLists)
  {
    if (!commandList)
      continue;
    ezSharedPtr<VKCommandList> vkCommandList = commandList.Downcast<VKCommandList>();
    vkCommandLists.PushBack(vkCommandList->GetCommandList());
  }

  vk::SubmitInfo submitInfo = {};
  submitInfo.commandBufferCount = vkCommandLists.GetCount();
  submitInfo.pCommandBuffers = vkCommandLists.GetData();

  vk::PipelineStageFlags waitDstStageMask = vk::PipelineStageFlagBits::eAllCommands;
  submitInfo.pWaitDstStageMask = &waitDstStageMask;

  vk::Result result = m_Queue.submit(1, &submitInfo, {});
}

VKDevice& VKCommandQueue::GetDevice()
{
  return m_Device;
}

ezUInt32 VKCommandQueue::GetQueueFamilyIndex()
{
  return m_QueueFamilyIndex;
}

vk::Queue VKCommandQueue::GetQueue()
{
  return m_Queue;
}
