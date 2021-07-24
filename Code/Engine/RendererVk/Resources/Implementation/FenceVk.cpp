#include <RendererVkPCH.h>

#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/RendererVkDLL.h>
#include <RendererVk/Resources/FenceVk.h>

ezGALFenceVk::ezGALFenceVk()
{
}

ezGALFenceVk::~ezGALFenceVk() {}


ezResult ezGALFenceVk::InitPlatform(ezGALDevice* pDevice, ezUInt64 initialValue)
{
  m_pDevice = static_cast<ezGALDeviceVk*>(pDevice);

  vk::SemaphoreTypeCreateInfo timelineCreateInfo = {};
  timelineCreateInfo.initialValue = initialValue;
  timelineCreateInfo.semaphoreType = vk::SemaphoreType::eTimeline;
  vk::SemaphoreCreateInfo create_info;
  create_info.pNext = &timelineCreateInfo;
  m_TimelineSemaphore = m_pDevice->GetVkDevice().createSemaphoreUnique(create_info);

  return EZ_SUCCESS;
}

ezResult ezGALFenceVk::DeInitPlatform(ezGALDevice* pDevice)
{
  return EZ_SUCCESS;
}

ezUInt64 ezGALFenceVk::GetCompletedValuePlatform()
{
  return m_pDevice->GetVkDevice().getSemaphoreCounterValueKHR(m_TimelineSemaphore.get());
}

void ezGALFenceVk::WaitPlatform(ezUInt64 value)
{
  vk::SemaphoreWaitInfo waitInfo = {};
  waitInfo.semaphoreCount = 1;
  waitInfo.pSemaphores = &m_TimelineSemaphore.get();
  waitInfo.pValues = &value;
  vk::Result result = m_pDevice->GetVkDevice().waitSemaphoresKHR(waitInfo, UINT64_MAX);
}

void ezGALFenceVk::SignalPlatform(ezUInt64 value)
{
  vk::SemaphoreSignalInfo signalInfo = {};
  signalInfo.semaphore = m_TimelineSemaphore.get();
  signalInfo.value = value;
  m_pDevice->GetVkDevice().signalSemaphoreKHR(signalInfo);
}


EZ_STATICLINK_FILE(RendererVk, RendererVk_Resources_Implementation_FenceVk);
