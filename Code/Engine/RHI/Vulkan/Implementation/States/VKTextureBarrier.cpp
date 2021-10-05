#include <RHI/Vulkan/States/VKTextureBarrier.h>
#include <RHI/Vulkan/VKGPUObjects.h>
#include <RHI/Vulkan/VKQueue.h>

CCVKTextureBarrier::CCVKTextureBarrier(const TextureBarrierInfo& info, ezUInt32 hash)
  : TextureBarrier(info, hash)
{
  m_TypedID = GenerateObjectID<decltype(this)>();

  _gpuBarrier = EZ_DEFAULT_NEW(CCVKGPUTextureBarrier);
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

  _gpuBarrier->barrier.prevLayout = _gpuBarrier->barrier.nextLayout = THSVS_IMAGE_LAYOUT_OPTIMAL;
  _gpuBarrier->barrier.discardContents = info.discardContents;
  _gpuBarrier->barrier.subresourceRange.baseMipLevel = 0U;
  _gpuBarrier->barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
  _gpuBarrier->barrier.subresourceRange.baseArrayLayer = 0U;
  _gpuBarrier->barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
  _gpuBarrier->barrier.srcQueueFamilyIndex = info.srcQueue
                                               ? static_cast<CCVKQueue*>(info.srcQueue)->gpuQueue()->queueFamilyIndex
                                               : VK_QUEUE_FAMILY_IGNORED;
  _gpuBarrier->barrier.dstQueueFamilyIndex = info.dstQueue
                                               ? static_cast<CCVKQueue*>(info.dstQueue)->gpuQueue()->queueFamilyIndex
                                               : VK_QUEUE_FAMILY_IGNORED;

  thsvsGetVulkanImageMemoryBarrier(_gpuBarrier->barrier, &_gpuBarrier->srcStageMask, &_gpuBarrier->dstStageMask, &_gpuBarrier->vkBarrier);
}

CCVKTextureBarrier::~CCVKTextureBarrier()
{
  EZ_DEFAULT_DELETE(_gpuBarrier);
}
