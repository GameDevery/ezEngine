#pragma once

#include <RHI/RHIDLL.h>

class EZ_RHI_DLL ezRHIInstance : public ezRefCounted
{
public:
  virtual ~ezRHIInstance() = default;
  virtual ezDynamicArray<ezSharedPtr<ezRHIAdapter>> EnumerateAdapters() = 0;

protected:
  ezRHIInstance() = default;
};
