#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/CommandQueue/CommandQueue.h>

class VKDevice;

class VKCommandQueue : public CommandQueue
{
public:
    VKCommandQueue(VKDevice& device, CommandListType type, ezUInt32 queueFamilyIndex);
    void Wait(const std::shared_ptr<Fence>& fence, ezUInt64 value) override;
    void Signal(const std::shared_ptr<Fence>& fence, ezUInt64 value) override;
    void ExecuteCommandLists(const std::vector<ezSharedPtr<CommandList>>& commandLists) override;

    VKDevice& GetDevice();
    ezUInt32 GetQueueFamilyIndex();
    vk::Queue GetQueue();

private:
    VKDevice& m_Device;
    ezUInt32 m_QueueFamilyIndex;
    vk::Queue m_Queue;
};
