#pragma once

#include <RendererVk/RendererVkDLL.h>

#include <RendererFoundation/Resources/Fence.h>
#include <vulkan/vulkan.hpp>

class EZ_RENDERERVK_DLL ezGALFenceVk : public ezGALFence
{
public:
  EZ_ALWAYS_INLINE const vk::Semaphore& GetVkFence() const { return m_TimelineSemaphore.get(); }

protected:
  friend class ezGALDeviceVk;
  friend class ezMemoryUtils;

  ezGALFenceVk();

  virtual ~ezGALFenceVk();

  virtual ezResult InitPlatform(ezGALDevice* pDevice, ezUInt64 initialValue) override;

  virtual ezResult DeInitPlatform(ezGALDevice* pDevice) override;

  virtual ezUInt64 GetCompletedValuePlatform() override;
  virtual void WaitPlatform(ezUInt64 value) override;
  virtual void SignalPlatform(ezUInt64 value) override;

private:
  ezGALDeviceVk* m_pDevice;
  vk::UniqueSemaphore m_TimelineSemaphore;
};
