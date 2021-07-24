#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Fence/Fence.h>

class VKDevice;

class VKTimelineSemaphore : public Fence
{
public:
    VKTimelineSemaphore(VKDevice& device, ezUInt64 initialValue);
    ezUInt64 GetCompletedValue() override;
    void Wait(ezUInt64 value) override;
    void Signal(ezUInt64 value) override;

    const vk::Semaphore& GetFence() const;

private:
    VKDevice& m_Device;
    vk::UniqueSemaphore m_TimelineSemaphore;
};
