#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/Resource/Resource.h>

class EZ_RHI_DLL ResourceBase : public Resource
{
public:
  ResourceBase();

  ResourceType GetResourceType() const override final;
  ezRHIResourceFormat::Enum GetFormat() const override final;
  MemoryType GetMemoryType() const override final;

  void UpdateUploadBuffer(ezUInt64 buffer_offset, const void* data, ezUInt64 num_bytes) override final;
  void UpdateUploadBufferWithTextureData(ezUInt64 buffer_offset, ezUInt32 buffer_row_pitch, ezUInt32 buffer_depth_pitch,
    const void* src_data, ezUInt32 src_row_pitch, ezUInt32 src_depth_pitch, ezUInt32 num_rows, ezUInt32 num_slices) override final;
  ResourceState GetInitialState() const override final;
  bool IsBackBuffer() const override final;
  void SetInitialState(ResourceState state);
  ResourceStateTracker& GetGlobalResourceStateTracker();
  const ResourceStateTracker& GetGlobalResourceStateTracker() const;

  ezRHIResourceFormat::Enum format = ezRHIResourceFormat::UNKNOWN;
  ResourceType resource_type = ResourceType::kUnknown;
  ezSharedPtr<Resource> acceleration_structures_memory;
  bool is_back_buffer = false;

protected:
  ezSharedPtr<Memory> m_memory;
  MemoryType m_memory_type = MemoryType::kDefault;

private:
  ResourceStateTracker m_resource_state_tracker;
  ResourceState m_initial_state = ResourceState::kUnknown;
};
