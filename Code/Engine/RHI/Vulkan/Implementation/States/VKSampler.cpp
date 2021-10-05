#include <RHI/Vulkan/States/VKSampler.h>
#include <RHI/Vulkan/VKCommands.h>
#include <RHI/Vulkan/VKDevice.h>

CCVKSampler::CCVKSampler(const SamplerInfo& info, ezUInt32 hash)
  : Sampler(info, hash)
{
  m_TypedID = GenerateObjectID<decltype(this)>();

  _gpuSampler = EZ_DEFAULT_NEW(CCVKGPUSampler);
  _gpuSampler->minFilter = _info.minFilter;
  _gpuSampler->magFilter = _info.magFilter;
  _gpuSampler->mipFilter = _info.mipFilter;
  _gpuSampler->addressU = _info.addressU;
  _gpuSampler->addressV = _info.addressV;
  _gpuSampler->addressW = _info.addressW;
  _gpuSampler->maxAnisotropy = _info.maxAnisotropy;
  _gpuSampler->cmpFunc = _info.cmpFunc;

  cmdFuncCCVKCreateSampler(CCVKDevice::getInstance(), _gpuSampler);
}

CCVKSampler::~CCVKSampler()
{ // NOLINT(bugprone-exception-escape) garbage collect may throw
  if (_gpuSampler)
  {
    CCVKDevice::getInstance()->gpuDescriptorHub()->disengage(_gpuSampler);
    CCVKDevice::getInstance()->gpuRecycleBin()->collect(_gpuSampler);
    _gpuSampler = nullptr;
  }
}
