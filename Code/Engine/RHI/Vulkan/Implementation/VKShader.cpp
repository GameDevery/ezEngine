#include <RHI/Vulkan/VKStd.h>

#include <RHI/Vulkan/VKCommands.h>
#include <RHI/Vulkan/VKDevice.h>
#include <RHI/Vulkan/VKShader.h>

CCVKShader::CCVKShader()
{
  m_TypedID = GenerateObjectID<decltype(this)>();
}

CCVKShader::~CCVKShader()
{
  destroy();
}

void CCVKShader::doInit(const ShaderInfo& /*info*/)
{
  _gpuShader = EZ_DEFAULT_NEW(CCVKGPUShader);
  _gpuShader->name = _name;
  _gpuShader->attributes = _attributes;
  _gpuShader->blocks = _blocks;
  _gpuShader->samplers = _samplers;
  for (ShaderStage& stage : _stages)
  {
    _gpuShader->gpuStages.PushBack({stage.stage, stage.source});
  }

  cmdFuncCCVKCreateShader(CCVKDevice::getInstance(), _gpuShader);
}

void CCVKShader::doDestroy()
{
  if (_gpuShader)
  {
    CCVKDevice::getInstance()->gpuRecycleBin()->collect(_gpuShader);
    _gpuShader = nullptr;
  }
}
