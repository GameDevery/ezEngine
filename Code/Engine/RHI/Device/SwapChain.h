#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Descriptors/Descriptors.h>

class EZ_RHI_DLL ezRHISwapChain : public ezRHIObject<ezRHISwapChainCreationDescription>
{
public:
  virtual ~ezRHISwapChain() = default;
  virtual ezRHIResourceFormat GetFormat() const = 0;
  virtual ezRHIResourceHandle GetBackBuffer(ezUInt32 buffer) = 0;
  virtual ezUInt32 NextImage(ezRHIFenceHandle hFence, ezUInt64 signalValue) = 0;
  virtual void Present(ezRHIFenceHandle hFence, ezUInt64 waitValue) = 0;
};
