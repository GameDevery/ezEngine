#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/BindingSet/BindingSet.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUDescriptorPool.h>

class VKDevice;
class VKBindingSetLayout;

class VKBindingSet
    : public BindingSet
{
public:
    VKBindingSet(VKDevice& device, const ezSharedPtr<VKBindingSetLayout>& layout);

    void WriteBindings(const std::vector<BindingDesc>& bindings) override;
    const ezDynamicArray<vk::DescriptorSet>& GetDescriptorSets() const;

private:
    VKDevice& m_Device;
    ezDynamicArray<DescriptorSetPool> m_Descriptors;
    ezDynamicArray<vk::DescriptorSet> m_DescriptorSets;
    ezSharedPtr<VKBindingSetLayout> m_Layout;
};
