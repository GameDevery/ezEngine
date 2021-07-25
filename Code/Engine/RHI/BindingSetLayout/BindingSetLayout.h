#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>
#include <vector>

class EZ_RHI_DLL BindingSetLayout : public ezRefCounted
{
public:
  virtual ~BindingSetLayout() = default;
};
