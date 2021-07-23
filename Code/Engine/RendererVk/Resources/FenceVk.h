
#pragma once

#include <RendererFoundation/Resources/Fence.h>

class EZ_RENDERERVK_DLL ezGALFenceVk : public ezGALFence
{
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
};
