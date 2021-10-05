#include <RHI/Vulkan/VKStd.h>

#include <RHI/Base/Def.h>

#include <RHI/Vulkan/VKCommands.h>
#include <RHI/Vulkan/VKDescriptorSetLayout.h>
#include <RHI/Vulkan/VKDevice.h>

CCVKDescriptorSetLayout::CCVKDescriptorSetLayout()
{
  m_TypedID = GenerateObjectID<decltype(this)>();
}

CCVKDescriptorSetLayout::~CCVKDescriptorSetLayout()
{
  destroy();
}

void CCVKDescriptorSetLayout::doInit(const DescriptorSetLayoutInfo& /*info*/)
{
  _gpuDescriptorSetLayout = EZ_DEFAULT_NEW(CCVKGPUDescriptorSetLayout);
  _gpuDescriptorSetLayout->id = generateID();
  _gpuDescriptorSetLayout->descriptorCount = _descriptorCount;
  _gpuDescriptorSetLayout->bindingIndices = _bindingIndices;
  _gpuDescriptorSetLayout->descriptorIndices = _descriptorIndices;
  _gpuDescriptorSetLayout->bindings = _bindings;

  for (auto& binding : _bindings)
  {
    if (HasAnyFlags(binding.descriptorType, DESCRIPTOR_DYNAMIC_TYPE))
    {
      for (ezUInt32 j = 0U; j < binding.count; j++)
      {
        _gpuDescriptorSetLayout->dynamicBindings.PushBack(binding.binding);
      }
    }
  }

  cmdFuncCCVKCreateDescriptorSetLayout(CCVKDevice::getInstance(), _gpuDescriptorSetLayout);
}

void CCVKDescriptorSetLayout::doDestroy()
{
  if (_gpuDescriptorSetLayout)
  {
    CCVKDevice::getInstance()->gpuRecycleBin()->collect(_gpuDescriptorSetLayout);
    _gpuDescriptorSetLayout = nullptr;
  }
}
