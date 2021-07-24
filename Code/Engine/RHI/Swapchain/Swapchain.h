#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Fence/Fence.h>
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Resource/Resource.h>

using Window = void*;

class EZ_RHI_DLL Swapchain : public ezRefCounted
{
public:
  virtual ~Swapchain() = default;
  virtual ezRHIResourceFormat::Enum GetFormat() const = 0;
  virtual std::shared_ptr<Resource> GetBackBuffer(ezUInt32 buffer) = 0;
  virtual ezUInt32 NextImage(const std::shared_ptr<Fence>& fence, ezUInt64 signalValue) = 0;
  virtual void Present(const std::shared_ptr<Fence>& fence, ezUInt64 waitValue) = 0;
};
