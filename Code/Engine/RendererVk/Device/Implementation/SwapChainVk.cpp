#include <RendererVkPCH.h>

#include <Core/System/Window.h>
#include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#include <RendererVk/Device/DeviceVk.h>
#include <RendererVk/Device/SwapChainVk.h>

#include <Foundation/Basics/Platform/Win/HResultUtils.h>

ezGALSwapChainVk::ezGALSwapChainVk(const ezGALSwapChainCreationDescription& Description)
  : ezGALSwapChain(Description)
  //, m_VkSurface{VK_NULL_HANDLE}
  //, m_VkSwapChain{VK_NULL_HANDLE}
  //, m_VkSwapChainImageFormat{}
  //, m_VkSwapChainExtent{}
  , m_pDeviceVk{nullptr}
{
}

ezGALSwapChainVk::~ezGALSwapChainVk() {}

ezResult ezGALSwapChainVk::InitPlatform(ezGALDevice* pDevice)
{
  m_pDeviceVk = static_cast<ezGALDeviceVk*>(pDevice);


  return EZ_SUCCESS;
}

ezResult ezGALSwapChainVk::DeInitPlatform(ezGALDevice* pDevice)
{
  if (m_pDeviceVk)
  {

  }
  return EZ_SUCCESS;
}



EZ_STATICLINK_FILE(RendererVk, RendererVk_Device_Implementation_SwapChainVk);
