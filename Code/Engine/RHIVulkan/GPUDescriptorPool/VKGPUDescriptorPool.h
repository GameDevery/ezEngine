#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>
#include <algorithm>

class VKDevice;

struct DescriptorSetPool
{
    vk::UniqueDescriptorPool pool;
    vk::UniqueDescriptorSet set;
};

class VKGPUDescriptorPool
{
public:
    VKGPUDescriptorPool(VKDevice& device);
    DescriptorSetPool AllocateDescriptorSet(const vk::DescriptorSetLayout& set_layout, const ezMap<vk::DescriptorType, ezUInt32>& count);

private:
    vk::UniqueDescriptorPool CreateDescriptorPool(const ezMap<vk::DescriptorType, ezUInt32>& count);

    VKDevice& m_device;
};
