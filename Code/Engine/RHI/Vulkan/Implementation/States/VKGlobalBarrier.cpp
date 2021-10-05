#include <RHI/Vulkan/States/VKGlobalBarrier.h>
#include <RHI/Vulkan/VKCommands.h>

CCVKGlobalBarrier::CCVKGlobalBarrier(const GlobalBarrierInfo& info, ezUInt32 hash)
  : GlobalBarrier(info, hash)
{
  m_TypedID = GenerateObjectID<decltype(this)>();

  _gpuBarrier = EZ_DEFAULT_NEW(CCVKGPUGlobalBarrier);
  _gpuBarrier->accessTypes.SetCount(info.prevAccesses.GetCount() + info.nextAccesses.GetCount());

  ezUInt32 index = 0U;
  for (AccessType type : info.prevAccesses)
  {
    _gpuBarrier->accessTypes[index++] = THSVS_ACCESS_TYPES[static_cast<ezUInt32>(type)];
  }
  for (AccessType type : info.nextAccesses)
  {
    _gpuBarrier->accessTypes[index++] = THSVS_ACCESS_TYPES[static_cast<ezUInt32>(type)];
  }

  _gpuBarrier->barrier.prevAccessCount = info.prevAccesses.GetCount();
  _gpuBarrier->barrier.pPrevAccesses = _gpuBarrier->accessTypes.GetData();
  _gpuBarrier->barrier.nextAccessCount = info.nextAccesses.GetCount();
  _gpuBarrier->barrier.pNextAccesses = _gpuBarrier->accessTypes.GetData() + info.prevAccesses.GetCount();

  thsvsGetVulkanMemoryBarrier(_gpuBarrier->barrier, &_gpuBarrier->srcStageMask, &_gpuBarrier->dstStageMask, &_gpuBarrier->vkBarrier);
}

CCVKGlobalBarrier::~CCVKGlobalBarrier()
{
  EZ_DEFAULT_DELETE(_gpuBarrier);
}
