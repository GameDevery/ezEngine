#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Memory/Memory.h>
#include <RHI/Instance/BaseTypes.h>

class VKDevice;

class VKMemory : public Memory
{
public:
    VKMemory(VKDevice& device, ezUInt64 size, MemoryType memory_type, ezUInt32 memory_type_bits, const vk::MemoryDedicatedAllocateInfoKHR* dedicated_allocate_info);
    MemoryType GetMemoryType() const override;
    vk::DeviceMemory GetMemory() const;

private:
    MemoryType m_memory_type;
    vk::UniqueDeviceMemory m_memory;
};
