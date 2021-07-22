#pragma once

#include <RHI/RHIDLL.h>

class EZ_RHI_DLL ezRHIMemory
{
public:
  virtual ~ezRHIMemory() = default;
  virtual ezRHIMemoryType GetMemoryType() const = 0;

protected:
  ezRHIMemory() = default;
};
