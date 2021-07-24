#pragma once

#include<RendererVk/RendererVkDLL.h>
#include <vulkan/vulkan.hpp>

namespace ezInternal::Vk
{
  class Adapter : public ezRefCounted
  {
  public:
    Adapter(Instance& instance, const vk::PhysicalDevice& physicalDevice);
    const ezString& GetName() const;
    Instance& GetInstance();
    vk::PhysicalDevice& GetPhysicalDevice();

  private:
    Instance& m_Instance;
    vk::PhysicalDevice m_PhysicalDevice;
    ezString m_Name;
  };
}
