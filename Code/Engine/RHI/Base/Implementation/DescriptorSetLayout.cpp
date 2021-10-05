#include <RHI/Base/DescriptorSetLayout.h>

DescriptorSetLayout::DescriptorSetLayout()
  : RHIObject(ObjectType::DESCRIPTOR_SET_LAYOUT)
{
}

DescriptorSetLayout::~DescriptorSetLayout() = default;

void DescriptorSetLayout::initialize(const DescriptorSetLayoutInfo& info)
{
  _bindings = info.bindings;
  auto bindingCount = _bindings.GetCount();
  _descriptorCount = 0U;

  if (bindingCount)
  {
    ezUInt32 maxBinding = 0U;
    ezDynamicArray<ezUInt32> flattenedIndices;
    flattenedIndices.SetCount(bindingCount);
    for (ezUInt32 i = 0U; i < bindingCount; i++)
    {
      const DescriptorSetLayoutBinding& binding = _bindings[i];
      if (binding.binding > maxBinding)
        maxBinding = binding.binding;
      flattenedIndices[i] = _descriptorCount;
      _descriptorCount += binding.count;
    }

    _bindingIndices.SetCount(maxBinding + 1, INVALID_BINDING);
    _descriptorIndices.SetCount(maxBinding + 1, INVALID_BINDING);
    for (ezUInt32 i = 0U; i < bindingCount; i++)
    {
      const DescriptorSetLayoutBinding& binding = _bindings[i];
      _bindingIndices[binding.binding] = i;
      _descriptorIndices[binding.binding] = flattenedIndices[i];
      if (HasFlag(DESCRIPTOR_DYNAMIC_TYPE, binding.descriptorType))
      {
        for (ezUInt32 j = 0U; j < binding.count; ++j)
        {
          _dynamicBindings.PushBack(binding.binding);
        }
      }
    }
  }

  doInit(info);
}

void DescriptorSetLayout::destroy()
{
  doDestroy();

  _bindings.Clear();
  _descriptorCount = 0U;
  _bindingIndices.Clear();
  _descriptorIndices.Clear();
}
