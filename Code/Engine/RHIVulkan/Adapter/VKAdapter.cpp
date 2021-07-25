#include <RHIVulkanPCH.h>

#include <RHIVulkan/Adapter/VKAdapter.h>
#include <RHIVulkan/Device/VKDevice.h>

VKAdapter::VKAdapter(VKInstance& instance, const vk::PhysicalDevice& physicalDevice)
  : m_Instance(instance)
  , m_PhysicalDevice(physicalDevice)
  , m_Name(physicalDevice.getProperties().deviceName.data())
{
}

const ezString& VKAdapter::GetName() const
{
  return m_Name;
}

ezSharedPtr<Device> VKAdapter::CreateDevice()
{
  return EZ_DEFAULT_NEW(VKDevice, *this);
}

VKInstance& VKAdapter::GetInstance()
{
  return m_Instance;
}

vk::PhysicalDevice& VKAdapter::GetPhysicalDevice()
{
  return m_PhysicalDevice;
}
