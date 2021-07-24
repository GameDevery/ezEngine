#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/CommandQueue/CommandQueue.h>

class VKDevice;

class VKCommandQueue : public CommandQueue
{
public:
    VKCommandQueue(VKDevice& device, CommandListType type, ezUInt32 queue_family_index);
    void Wait(const std::shared_ptr<Fence>& fence, ezUInt64 value) override;
    void Signal(const std::shared_ptr<Fence>& fence, ezUInt64 value) override;
    void ExecuteCommandLists(const std::vector<std::shared_ptr<CommandList>>& command_lists) override;

    VKDevice& GetDevice();
    ezUInt32 GetQueueFamilyIndex();
    vk::Queue GetQueue();

private:
    VKDevice& m_device;
    ezUInt32 m_queue_family_index;
    vk::Queue m_queue;
};
