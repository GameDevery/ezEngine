#pragma once

#include <RHI/RHIDLL.h>


class EZ_RHI_DLL ezRHIFence
{
public:
  virtual ~ezRHIFence() = default;
  virtual ezUInt64 GetCompletedValue() = 0;
  virtual void Wait(ezUInt64 value) = 0;
  virtual void Signal(ezUInt64 value) = 0;
};
