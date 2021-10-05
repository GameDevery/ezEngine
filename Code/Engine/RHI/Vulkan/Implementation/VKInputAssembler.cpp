#include <RHI/Vulkan/VKStd.h>

#include <RHI/Vulkan/VKBuffer.h>
#include <RHI/Vulkan/VKCommands.h>
#include <RHI/Vulkan/VKDevice.h>
#include <RHI/Vulkan/VKInputAssembler.h>

CCVKInputAssembler::CCVKInputAssembler()
{
  m_TypedID = GenerateObjectID<decltype(this)>();
}

CCVKInputAssembler::~CCVKInputAssembler()
{
  destroy();
}

void CCVKInputAssembler::doInit(const InputAssemblerInfo& info)
{
  ezUInt32 vbCount = _vertexBuffers.GetCount();

  _gpuInputAssembler = EZ_DEFAULT_NEW(CCVKGPUInputAssembler);
  _gpuInputAssembler->attributes = _attributes;
  _gpuInputAssembler->gpuVertexBuffers.SetCount(vbCount);

  for (ezUInt32 i = 0U; i < vbCount; ++i)
  {
    auto* vb = static_cast<CCVKBuffer*>(_vertexBuffers[i]);
    _gpuInputAssembler->gpuVertexBuffers[i] = vb->gpuBufferView();
  }

  if (info.indexBuffer)
  {
    _gpuInputAssembler->gpuIndexBuffer = static_cast<CCVKBuffer*>(info.indexBuffer)->gpuBufferView();
  }

  if (info.indirectBuffer)
  {
    _gpuInputAssembler->gpuIndirectBuffer = static_cast<CCVKBuffer*>(info.indirectBuffer)->gpuBufferView();
  }

  _gpuInputAssembler->vertexBuffers.SetCount(vbCount);
  _gpuInputAssembler->vertexBufferOffsets.SetCount(vbCount);

  CCVKGPUDevice* gpuDevice = CCVKDevice::getInstance()->gpuDevice();
  for (ezUInt32 i = 0U; i < vbCount; i++)
  {
    _gpuInputAssembler->vertexBuffers[i] = _gpuInputAssembler->gpuVertexBuffers[i]->gpuBuffer->vkBuffer;
    _gpuInputAssembler->vertexBufferOffsets[i] = _gpuInputAssembler->gpuVertexBuffers[i]->getStartOffset(gpuDevice->curBackBufferIndex);
  }
}

void CCVKInputAssembler::doDestroy()
{
  if (_gpuInputAssembler)
  {
    _gpuInputAssembler->vertexBuffers.Clear();
    _gpuInputAssembler->vertexBufferOffsets.Clear();
    EZ_DEFAULT_DELETE(_gpuInputAssembler);
    _gpuInputAssembler = nullptr;
  }
}
