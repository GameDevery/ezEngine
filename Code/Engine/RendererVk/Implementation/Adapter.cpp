#include <RendererVkPCH.h>

#include <RendererVk/Implementation/Adapter.h>

namespace ezInternal::Vk
{
  Adapter::Adapter(Instance& instance, const vk::PhysicalDevice& physicalDevice)
    : m_Instance{instance}
    , m_PhysicalDevice{physicalDevice}
    , m_Name{physicalDevice.getProperties().deviceName.data()}
  {
  }

  const ezString& ezInternal::Vk::Adapter::GetName() const
  {
    return m_Name;
  }

  Instance& ezInternal::Vk::Adapter::GetInstance()
  {
    return m_Instance;
  }

  vk::PhysicalDevice& ezInternal::Vk::Adapter::GetPhysicalDevice()
  {
    return m_PhysicalDevice;
  }
} // namespace ezInternal::Vk
