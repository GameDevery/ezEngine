#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Descriptors/Descriptors.h>

class EZ_RHI_DLL ezRHIRenderPass : public ezRHIObject<ezRHIRenderPassCreationDescription>
{
public:
  virtual ~ezRHIRenderPass() = default;
};
