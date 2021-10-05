#pragma once

#include <RHI/Base/Buffer.h>
#include <RHI/Vulkan/VKStd.h>

class CCVKGPUBuffer;
class CCVKGPUBufferView;

class EZ_VULKAN_API CCVKBuffer final : public Buffer
{
public:
  CCVKBuffer();
  ~CCVKBuffer() override;

  void update(const void* buffer, ezUInt32 size) override;

  inline CCVKGPUBuffer* gpuBuffer() const { return _gpuBuffer; }
  inline CCVKGPUBufferView* gpuBufferView() const { return _gpuBufferView; }

protected:
  void doInit(const BufferInfo& info) override;
  void doInit(const BufferViewInfo& info) override;
  void doDestroy() override;
  void doResize(ezUInt32 size, ezUInt32 count) override;

  void createBufferView();

  CCVKGPUBuffer* _gpuBuffer = nullptr;
  CCVKGPUBufferView* _gpuBufferView = nullptr;
};
