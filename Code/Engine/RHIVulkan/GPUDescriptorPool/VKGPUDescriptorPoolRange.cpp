#include <RHIVulkanPCH.h>

#include <RHIVulkan/GPUDescriptorPool/VKGPUDescriptorPoolRange.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUBindlessDescriptorPoolTyped.h>

VKGPUDescriptorPoolRange::VKGPUDescriptorPoolRange(VKGPUBindlessDescriptorPoolTyped& pool,
                                                   vk::DescriptorSet descriptor_set,
                                                   ezUInt32 offset,
                                                   ezUInt32 size,
                                                   vk::DescriptorType type)
    : m_pool(pool)
    , m_descriptor_set(descriptor_set)
    , m_offset(offset)
    , m_size(size)
    , m_type(type)
    //, m_Callback(this, [m_offset = m_offset, m_size = m_size, m_pool = m_pool](auto) { m_pool.get().OnRangeDestroy(m_offset, m_size); })
{
}

VKGPUDescriptorPoolRange::~VKGPUDescriptorPoolRange()
{
  m_pool.get().OnRangeDestroy(m_offset, m_size); 
}

vk::DescriptorSet VKGPUDescriptorPoolRange::GetDescriptoSet() const
{
    return m_descriptor_set;
}

ezUInt32 VKGPUDescriptorPoolRange::GetOffset() const
{
    return m_offset;
}
