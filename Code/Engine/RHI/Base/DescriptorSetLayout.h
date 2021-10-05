#pragma once

#include <RHI/Base/Object.h>

class EZ_RHI_DLL DescriptorSetLayout : public RHIObject
{
public:
  DescriptorSetLayout();
  ~DescriptorSetLayout() override;

  void initialize(const DescriptorSetLayoutInfo& info);
  void destroy();

  inline const DescriptorSetLayoutBindingList& getBindings() const { return _bindings; }
  inline const ezDynamicArray<ezUInt32>& getDynamicBindings() const { return _dynamicBindings; }
  inline const ezDynamicArray<ezUInt32>& getBindingIndices() const { return _bindingIndices; }
  inline const ezDynamicArray<ezUInt32>& getDescriptorIndices() const { return _descriptorIndices; }
  inline ezUInt32 getDescriptorCount() const { return _descriptorCount; }

protected:
  virtual void doInit(const DescriptorSetLayoutInfo& info) = 0;
  virtual void doDestroy() = 0;

  DescriptorSetLayoutBindingList _bindings;
  ezUInt32 _descriptorCount = 0U;
  ezDynamicArray<ezUInt32> _bindingIndices;
  ezDynamicArray<ezUInt32> _descriptorIndices;
  ezDynamicArray<ezUInt32> _dynamicBindings;
};
