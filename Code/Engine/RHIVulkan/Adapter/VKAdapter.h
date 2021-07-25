#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Adapter/Adapter.h>

class VKInstance;

class VKAdapter : public Adapter
{
public:
    VKAdapter(VKInstance& instance, const vk::PhysicalDevice& physicalDevice);
    const ezString& GetName() const override;
    ezSharedPtr<Device> CreateDevice() override;
    VKInstance& GetInstance();
    vk::PhysicalDevice& GetPhysicalDevice();

private:
    VKInstance& m_Instance;
    vk::PhysicalDevice m_PhysicalDevice;
    ezString m_Name;
};
