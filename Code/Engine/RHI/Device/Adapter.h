#pragma once

#include <RHI/RHIDLL.h>

class EZ_RHI_DLL ezRHIAdapter
{
public:
  virtual ~ezRHIAdapter() = default;
  virtual const ezString& GetName() const = 0;
  virtual ezSharedPtr<ezRHIDevice> CreateDevice() = 0;

protected:
  ezRHIAdapter() = default;
};
