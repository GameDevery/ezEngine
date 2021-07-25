#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>

class EZ_RHI_DLL QueryHeap : public ezRefCounted
{
public:
  virtual ~QueryHeap() = default;
  virtual QueryHeapType GetType() const = 0;
};
