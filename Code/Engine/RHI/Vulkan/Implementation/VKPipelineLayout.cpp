#include <RHI/Vulkan/VKStd.h>

#include <RHI/Vulkan/VKCommands.h>
#include <RHI/Vulkan/VKDescriptorSetLayout.h>
#include <RHI/Vulkan/VKDevice.h>
#include <RHI/Vulkan/VKPipelineLayout.h>


CCVKPipelineLayout::CCVKPipelineLayout()
{
  m_TypedID = GenerateObjectID<decltype(this)>();
}

CCVKPipelineLayout::~CCVKPipelineLayout()
{
  destroy();
}

void CCVKPipelineLayout::doInit(const PipelineLayoutInfo& /*info*/)
{
  _gpuPipelineLayout = EZ_DEFAULT_NEW(CCVKGPUPipelineLayout);

  ezUInt32 offset = 0U;
  for (auto* setLayout : _setLayouts)
  {
    CCVKGPUDescriptorSetLayout* gpuSetLayout = static_cast<CCVKDescriptorSetLayout*>(setLayout)->gpuDescriptorSetLayout();
    ezUInt32 dynamicCount = gpuSetLayout->dynamicBindings.GetCount();
    _gpuPipelineLayout->dynamicOffsetOffsets.PushBack(offset);
    _gpuPipelineLayout->setLayouts.PushBack(gpuSetLayout);
    offset += dynamicCount;
  }
  _gpuPipelineLayout->dynamicOffsetOffsets.PushBack(offset);
  _gpuPipelineLayout->dynamicOffsetCount = offset;

  cmdFuncCCVKCreatePipelineLayout(CCVKDevice::getInstance(), _gpuPipelineLayout);
}

void CCVKPipelineLayout::doDestroy()
{
  if (_gpuPipelineLayout)
  {
    CCVKDevice::getInstance()->gpuRecycleBin()->collect(_gpuPipelineLayout);
    _gpuPipelineLayout = nullptr;
  }
}
