#pragma once

#include <RHI/RHIDLL.h>

#include <RHI/Descriptors/Descriptors.h>

class ezRHIPipeline
{
public:
  virtual ~ezRHIPipeline() = default;
  virtual ezRHIPipelineType GetPipelineType() const = 0;
  virtual ezDynamicArray<ezUInt8> GetRayTracingShaderGroupHandles(ezUInt32 firstGroup, ezUInt32 groupCount) const = 0;
};
