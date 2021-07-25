#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Device/Device.h>

class EZ_RHI_DLL Adapter : public ezRefCounted
{
public:
  virtual ~Adapter() = default;
  virtual const ezString& GetName() const = 0;
  virtual ezSharedPtr<Device> CreateDevice() = 0;
};
