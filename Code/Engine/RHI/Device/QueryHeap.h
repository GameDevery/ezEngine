#pragma once

#include <RHI/RHIDLL.h>

class EZ_RHI_DLL ezRHIQueryHeap
{
public:
  virtual ~ezRHIQueryHeap() = default;
  virtual ezRHIQueryHeapType GetType() const = 0;
};
