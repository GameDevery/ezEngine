#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Instance/Instance.h>

class VKInstance : public Instance
{
public:
    VKInstance();
    ezDynamicArray<ezSharedPtr<Adapter>> EnumerateAdapters() override;
    vk::Instance& GetInstance();

private:
    vk::DynamicLoader m_DynamicLoader;
    vk::UniqueInstance m_Instance;
    vk::UniqueDebugReportCallbackEXT m_Callback;
};
