#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Adapter/Adapter.h>

class VKInstance;

class VKAdapter : public Adapter
{
public:
    VKAdapter(VKInstance& instance, const vk::PhysicalDevice& physical_device);
    const ezString& GetName() const override;
    ezSharedPtr<Device> CreateDevice() override;
    VKInstance& GetInstance();
    vk::PhysicalDevice& GetPhysicalDevice();

private:
    VKInstance& m_instance;
    vk::PhysicalDevice m_physical_device;
    ezString m_name;
};
