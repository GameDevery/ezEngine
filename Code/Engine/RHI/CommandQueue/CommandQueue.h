#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/CommandList/CommandList.h>
#include <RHI/Fence/Fence.h>

class EZ_RHI_DLL CommandQueue : public ezRefCounted
{
public:
  virtual ~CommandQueue() = default;
  virtual void Wait(const ezSharedPtr<Fence>& fence, ezUInt64 value) = 0;
  virtual void Signal(const ezSharedPtr<Fence>& fence, ezUInt64 value) = 0;
  virtual void ExecuteCommandLists(const std::vector<ezSharedPtr<CommandList>>& commandLists) = 0;
};
