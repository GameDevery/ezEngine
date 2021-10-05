#pragma once

#include <RHI/Base/Object.h>

class EZ_RHI_DLL PipelineLayout : public RHIObject
{
public:
  PipelineLayout();
  ~PipelineLayout() override;

  void initialize(const PipelineLayoutInfo& info);
  void destroy();

  inline const DescriptorSetLayoutList& getSetLayouts() const { return _setLayouts; }

protected:
  virtual void doInit(const PipelineLayoutInfo& info) = 0;
  virtual void doDestroy() = 0;

  DescriptorSetLayoutList _setLayouts;
};
