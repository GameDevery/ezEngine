#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>

class EZ_RHI_DLL RenderPass : public ezRefCounted
{
public:
  virtual ~RenderPass() = default;
  virtual const RenderPassDesc& GetDesc() const = 0;
};
