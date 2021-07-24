#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Instance/QueryInterface.h>
#include <RHI/Memory/Memory.h>
#include <RHI/Resource/ResourceStateTracker.h>
#include <RHI/View/View.h>
#include <memory>
#include <string>

struct EZ_RHI_DLL MemoryRequirements
{
  ezUInt64 size;
  ezUInt64 alignment;
  ezUInt32 memory_type_bits;
};

class EZ_RHI_DLL Resource : public QueryInterface
{
public:
  virtual ~Resource() = default;
  virtual void CommitMemory(MemoryType memory_type) = 0;
  virtual void BindMemory(const ezSharedPtr<Memory>& memory, ezUInt64 offset) = 0;
  virtual ResourceType GetResourceType() const = 0;
  virtual ezRHIResourceFormat::Enum GetFormat() const = 0;
  virtual MemoryType GetMemoryType() const = 0;
  virtual ezUInt64 GetWidth() const = 0;
  virtual ezUInt32 GetHeight() const = 0;
  virtual uint16_t GetLayerCount() const = 0;
  virtual uint16_t GetLevelCount() const = 0;
  virtual ezUInt32 GetSampleCount() const = 0;
  virtual ezUInt64 GetAccelerationStructureHandle() const = 0;
  virtual void SetName(const ezString& name) = 0;
  virtual ezUInt8* Map() = 0;
  virtual void Unmap() = 0;
  virtual void UpdateUploadBuffer(ezUInt64 buffer_offset, const void* data, ezUInt64 num_bytes) = 0;
  virtual void UpdateUploadBufferWithTextureData(ezUInt64 buffer_offset, ezUInt32 buffer_row_pitch, ezUInt32 buffer_depth_pitch,
    const void* src_data, ezUInt32 src_row_pitch, ezUInt32 src_depth_pitch, ezUInt32 num_rows, ezUInt32 num_slices) = 0;
  virtual bool AllowCommonStatePromotion(ResourceState state_after) = 0;
  virtual ResourceState GetInitialState() const = 0;
  virtual MemoryRequirements GetMemoryRequirements() const = 0;
  virtual bool IsBackBuffer() const = 0;
};
