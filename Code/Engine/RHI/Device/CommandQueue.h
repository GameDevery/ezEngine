#pragma once

#include <RHI/RHIDLL.h>

class EZ_RHI_DLL ezRHICommandQueue : public ezRefCounted
{
public:
  virtual ~ezRHICommandQueue() = default;
  virtual void Wait(ezRHIFenceHandle hFence, ezUInt64 value) = 0;
  virtual void Signal(ezRHIFenceHandle hFence, ezUInt64 value) = 0;
  virtual void ExecuteCommandList(ezRHICommandListHandle commandLists) = 0;
  virtual void ExecuteCommandLists(const ezDynamicArray<ezRHICommandList*>& commandLists) = 0;

protected:
  ezRHICommandQueue() = default;
};
