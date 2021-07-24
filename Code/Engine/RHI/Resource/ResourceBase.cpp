#include <RHI/Resource/ResourceBase.h>

ResourceBase::ResourceBase()
    : m_resource_state_tracker(*this)
{
}

ResourceType ResourceBase::GetResourceType() const
{
    return resource_type;
}

ezRHIResourceFormat::Enum ResourceBase::GetFormat() const
{
    return format;
}

MemoryType ResourceBase::GetMemoryType() const
{
    return m_memory_type;
}

void ResourceBase::UpdateUploadBuffer(ezUInt64 buffer_offset, const void* data, ezUInt64 num_bytes)
{
    void* dst_data = Map() + buffer_offset;
    memcpy(dst_data, data, num_bytes);
    Unmap();
}

void ResourceBase::UpdateUploadBufferWithTextureData(ezUInt64 buffer_offset, ezUInt32 buffer_row_pitch, ezUInt32 buffer_depth_pitch,
    const void* src_data, ezUInt32 src_row_pitch, ezUInt32 src_depth_pitch, ezUInt32 num_rows, ezUInt32 num_slices)
{
    void* dst_data = Map() + buffer_offset;
    for (ezUInt32 z = 0; z < num_slices; ++z)
    {
        ezUInt8* dest_slice = reinterpret_cast<ezUInt8*>(dst_data) + buffer_depth_pitch * z;
        const ezUInt8* src_slice = reinterpret_cast<const ezUInt8*>(src_data) + src_depth_pitch * z;
        for (ezUInt32 y = 0; y < num_rows; ++y)
        {
            memcpy(dest_slice + buffer_row_pitch * y, src_slice + src_row_pitch * y, src_row_pitch);
        }
    }
    Unmap();
}

ResourceState ResourceBase::GetInitialState() const
{
    return m_initial_state;
}

bool ResourceBase::IsBackBuffer() const
{
  return is_back_buffer;
}

void ResourceBase::SetInitialState(ResourceState state)
{
    m_initial_state = state;
    m_resource_state_tracker.SetResourceState(m_initial_state);
}

ResourceStateTracker& ResourceBase::GetGlobalResourceStateTracker()
{
    return m_resource_state_tracker;
}

const ResourceStateTracker& ResourceBase::GetGlobalResourceStateTracker() const
{
    return m_resource_state_tracker;
}
