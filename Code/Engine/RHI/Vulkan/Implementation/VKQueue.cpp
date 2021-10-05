#include <RHI/Vulkan/VKStd.h>

#include <RHI/Vulkan/VKCommandBuffer.h>
#include <RHI/Vulkan/VKCommands.h>
#include <RHI/Vulkan/VKDevice.h>
#include <RHI/Vulkan/VKQueue.h>
#include <vulkan/vulkan_core.h>


CCVKQueue::CCVKQueue()
{
  m_TypedID = GenerateObjectID<decltype(this)>();
}

CCVKQueue::~CCVKQueue()
{
  destroy();
}

void CCVKQueue::doInit(const QueueInfo& /*info*/)
{
  _gpuQueue = EZ_DEFAULT_NEW(CCVKGPUQueue);
  _gpuQueue->type = _type;
  cmdFuncCCVKGetDeviceQueue(CCVKDevice::getInstance(), _gpuQueue);
}

void CCVKQueue::doDestroy()
{
  if (_gpuQueue)
  {
    _gpuQueue->vkQueue = VK_NULL_HANDLE;
    EZ_DEFAULT_DELETE(_gpuQueue);
    _gpuQueue = nullptr;
  }
}

void CCVKQueue::submit(CommandBuffer* const* cmdBuffs, ezUInt32 count)
{
  CCVKDevice* device = CCVKDevice::getInstance();
  _gpuQueue->commandBuffers.Clear();

#if BARRIER_DEDUCTION_LEVEL >= BARRIER_DEDUCTION_LEVEL_BASIC
  device->gpuBarrierManager()->update(device->gpuTransportHub());
#endif
  device->gpuBufferHub()->flush(device->gpuTransportHub());

  if (!device->gpuTransportHub()->empty(false))
  {
    _gpuQueue->commandBuffers.PushBack(device->gpuTransportHub()->packageForFlight(false));
  }

  for (ezUInt32 i = 0U; i < count; ++i)
  {
    auto* cmdBuff = static_cast<CCVKCommandBuffer*>(cmdBuffs[i]);
    if (!cmdBuff->_pendingQueue.IsEmpty())
    {
      _gpuQueue->commandBuffers.PushBack(cmdBuff->_pendingQueue.PeekFront());
      cmdBuff->_pendingQueue.PopFront();

      _numDrawCalls += cmdBuff->_numDrawCalls;
      _numInstances += cmdBuff->_numInstances;
      _numTriangles += cmdBuff->_numTriangles;
    }
  }

  if (!device->gpuTransportHub()->empty(true))
  {
    _gpuQueue->commandBuffers.PushBack(device->gpuTransportHub()->packageForFlight(true));
  }

  ezUInt32 waitSemaphoreCount = _gpuQueue->lastSignaledSemaphores.GetCount();
  VkSemaphore signal = waitSemaphoreCount ? device->gpuSemaphorePool()->alloc() : VK_NULL_HANDLE;
  _gpuQueue->submitStageMasks.SetCount(waitSemaphoreCount, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

  VkSubmitInfo submitInfo{VK_STRUCTURE_TYPE_SUBMIT_INFO};
  submitInfo.waitSemaphoreCount = waitSemaphoreCount;
  submitInfo.pWaitSemaphores = _gpuQueue->lastSignaledSemaphores.GetData();
  submitInfo.pWaitDstStageMask = _gpuQueue->submitStageMasks.GetData();
  submitInfo.commandBufferCount = _gpuQueue->commandBuffers.GetCount();
  submitInfo.pCommandBuffers = &_gpuQueue->commandBuffers[0];
  submitInfo.signalSemaphoreCount = waitSemaphoreCount ? 1 : 0;
  submitInfo.pSignalSemaphores = &signal;

  VkFence vkFence = device->gpuFencePool()->alloc();
  VK_CHECK(vkQueueSubmit(_gpuQueue->vkQueue, 1, &submitInfo, vkFence));

  //_gpuQueue->lastSignaledSemaphores.assign(1, signal);
  {
    _gpuQueue->lastSignaledSemaphores.Clear();
    _gpuQueue->lastSignaledSemaphores.SetCount(1, signal);
  }
}
