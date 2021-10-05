#include <RHI/Vulkan/VKStd.h>

#include <RHI/Vulkan/VKBuffer.h>
#include <RHI/Vulkan/VKCommandBuffer.h>
#include <RHI/Vulkan/VKCommands.h>
#include <RHI/Vulkan/VKDevice.h>

CCVKBuffer::CCVKBuffer()
{
  m_TypedID = GenerateObjectID<decltype(this)>();
}

CCVKBuffer::~CCVKBuffer()
{
  destroy();
}

void CCVKBuffer::doInit(const BufferInfo& /*info*/)
{
  _gpuBuffer = EZ_DEFAULT_NEW(CCVKGPUBuffer);
  _gpuBuffer->usage = _usage;
  _gpuBuffer->memUsage = _memUsage;
  _gpuBuffer->size = _size;
  _gpuBuffer->stride = _stride;
  _gpuBuffer->count = _count;

  if (HasFlag(_usage, BufferUsageBit::INDIRECT))
  {
    const ezUInt32 drawInfoCount = _size / sizeof(DrawInfo);
    _gpuBuffer->indexedIndirectCmds.SetCount(drawInfoCount);
    _gpuBuffer->indirectCmds.SetCount(drawInfoCount);
  }

  cmdFuncCCVKCreateBuffer(CCVKDevice::getInstance(), _gpuBuffer);
  CCVKDevice::getInstance()->getMemoryStatus().bufferSize += _size;

  _gpuBufferView = EZ_DEFAULT_NEW(CCVKGPUBufferView);
  createBufferView();
}

void CCVKBuffer::doInit(const BufferViewInfo& info)
{
  auto* buffer = static_cast<CCVKBuffer*>(info.buffer);
  _gpuBuffer = buffer->gpuBuffer();
  _gpuBufferView = EZ_DEFAULT_NEW(CCVKGPUBufferView);
  createBufferView();
}

void CCVKBuffer::createBufferView()
{
  _gpuBufferView->gpuBuffer = _gpuBuffer;
  _gpuBufferView->offset = _offset;
  _gpuBufferView->range = _size;
  if (_gpuBufferView->gpuBuffer->vkBuffer)
  {
    CCVKDevice::getInstance()->gpuDescriptorHub()->update(_gpuBufferView);
  }
}

void CCVKBuffer::doDestroy()
{
  if (_gpuBufferView)
  {
    CCVKDevice::getInstance()->gpuDescriptorHub()->disengage(_gpuBufferView);
    EZ_DEFAULT_DELETE(_gpuBufferView);
    _gpuBufferView = nullptr;
  }

  if (_gpuBuffer)
  {
    if (!_isBufferView)
    {
      CCVKDevice::getInstance()->gpuBufferHub()->erase(_gpuBuffer);
      CCVKDevice::getInstance()->gpuRecycleBin()->collect(_gpuBuffer);
      CCVKDevice::getInstance()->gpuBarrierManager()->cancel(_gpuBuffer);
      EZ_DEFAULT_DELETE(_gpuBuffer);
      CCVKDevice::getInstance()->getMemoryStatus().bufferSize -= _size;
    }
    _gpuBuffer = nullptr;
  }
}

void CCVKBuffer::doResize(ezUInt32 size, ezUInt32 count)
{
  CCVKDevice::getInstance()->getMemoryStatus().bufferSize -= _size;
  CCVKDevice::getInstance()->gpuRecycleBin()->collect(_gpuBuffer);

  _gpuBuffer->size = size;
  _gpuBuffer->count = count;
  cmdFuncCCVKCreateBuffer(CCVKDevice::getInstance(), _gpuBuffer);

  _gpuBufferView->range = size;
  CCVKDevice::getInstance()->gpuDescriptorHub()->update(_gpuBuffer);

  if (HasFlag(_usage, BufferUsageBit::INDIRECT))
  {
    const ezUInt32 drawInfoCount = _size / sizeof(DrawInfo);
    _gpuBuffer->indexedIndirectCmds.SetCount(drawInfoCount);
    _gpuBuffer->indirectCmds.SetCount(drawInfoCount);
  }
  CCVKDevice::getInstance()->getMemoryStatus().bufferSize += size;
}

void CCVKBuffer::update(const void* buffer, ezUInt32 size)
{
  cmdFuncCCVKUpdateBuffer(CCVKDevice::getInstance(), _gpuBuffer, buffer, size, nullptr);
}
