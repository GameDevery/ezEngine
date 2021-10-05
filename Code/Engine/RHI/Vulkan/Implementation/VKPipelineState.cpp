#include <RHI/Vulkan/VKStd.h>

#include <RHI/Vulkan/VKCommands.h>
#include <RHI/Vulkan/VKDevice.h>
#include <RHI/Vulkan/VKPipelineLayout.h>
#include <RHI/Vulkan/VKPipelineState.h>
#include <RHI/Vulkan/VKRenderPass.h>
#include <RHI/Vulkan/VKShader.h>

CCVKPipelineState::CCVKPipelineState()
{
  m_TypedID = GenerateObjectID<decltype(this)>();
}

CCVKPipelineState::~CCVKPipelineState()
{
  destroy();
}

void CCVKPipelineState::doInit(const PipelineStateInfo& /*info*/)
{
  _gpuPipelineState = EZ_DEFAULT_NEW(CCVKGPUPipelineState);
  _gpuPipelineState->bindPoint = _bindPoint;
  _gpuPipelineState->primitive = _primitive;
  _gpuPipelineState->gpuShader = static_cast<CCVKShader*>(_shader)->gpuShader();
  _gpuPipelineState->inputState = _inputState;
  _gpuPipelineState->rs = _rasterizerState;
  _gpuPipelineState->dss = _depthStencilState;
  _gpuPipelineState->bs = _blendState;
  _gpuPipelineState->subpass = _subpass;
  _gpuPipelineState->gpuPipelineLayout = static_cast<CCVKPipelineLayout*>(_pipelineLayout)->gpuPipelineLayout();
  if (_renderPass)
    _gpuPipelineState->gpuRenderPass = static_cast<CCVKRenderPass*>(_renderPass)->gpuRenderPass();

  for (ezUInt32 i = 0; i < 31; i++)
  {
    if (static_cast<ezUInt32>(_dynamicStates) & (1 << i))
    {
      _gpuPipelineState->dynamicStates.PushBack(static_cast<DynamicStateFlagBit>(1 << i));
    }
  }

  if (_bindPoint == PipelineBindPoint::GRAPHICS)
  {
    cmdFuncCCVKCreateGraphicsPipelineState(CCVKDevice::getInstance(), _gpuPipelineState);
  }
  else
  {
    cmdFuncCCVKCreateComputePipelineState(CCVKDevice::getInstance(), _gpuPipelineState);
  }
}

void CCVKPipelineState::doDestroy()
{
  if (_gpuPipelineState)
  {
    CCVKDevice::getInstance()->gpuRecycleBin()->collect(_gpuPipelineState);
    _gpuPipelineState = nullptr;
  }
}
