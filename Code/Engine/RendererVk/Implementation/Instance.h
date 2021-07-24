#pragma once

#include <RendererVk/RendererVkDLL.h>
#include <vulkan/vulkan.hpp>

namespace ezInternal::Vk
{
  class Instance
  {
  public:
    Instance();
    ezDynamicArray<ezSharedPtr<Adapter>> EnumerateAdapters();
    vk::Instance GetInstance();

  private:
    vk::DynamicLoader m_DynamicLoader;
    vk::UniqueInstance m_Instance;
    vk::UniqueDebugReportCallbackEXT m_Callback;
  };
} // namespace ezInternal::Vk
