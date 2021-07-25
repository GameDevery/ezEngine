#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>

class EZ_RHI_DLL Memory : public ezRefCounted
{
public:
  virtual ~Memory() = default;
  virtual MemoryType GetMemoryType() const = 0;
};
