#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>
#include <RHI/Instance/QueryInterface.h>
#include <cstdint>

class EZ_RHI_DLL Fence : public QueryInterface
{
public:
  virtual ~Fence() = default;
  virtual ezUInt64 GetCompletedValue() = 0;
  virtual void Wait(ezUInt64 value) = 0;
  virtual void Signal(ezUInt64 value) = 0;
};
