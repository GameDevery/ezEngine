#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Descriptors/Descriptors.h>

class EZ_RHI_DLL ezRHIBindingSet
{
public:
  virtual ~ezRHIBindingSet() = default;
  virtual void WriteBindings(const ezDynamicArray<ezRHIBindingDescription>& bindings) = 0;
};
