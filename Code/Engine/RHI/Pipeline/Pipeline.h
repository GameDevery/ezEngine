#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/BaseTypes.h>
#include <RHI/Instance/QueryInterface.h>
#include <memory>

class EZ_RHI_DLL Pipeline : public QueryInterface
{
public:
  virtual ~Pipeline() = default;
  virtual PipelineType GetPipelineType() const = 0;
  virtual ezDynamicArray<ezUInt8> GetRayTracingShaderGroupHandles(ezUInt32 firstGroup, ezUInt32 groupCount) const = 0;
};
