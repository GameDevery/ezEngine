#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>

class EZ_RHI_DLL Fence : public ezRefCounted
{
public:
  virtual ~Fence() = default;
  virtual ezUInt64 GetCompletedValue() = 0;
  virtual void Wait(ezUInt64 value) = 0;
  virtual void Signal(ezUInt64 value) = 0;
};
