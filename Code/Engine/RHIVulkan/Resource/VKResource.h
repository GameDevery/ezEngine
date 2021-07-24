#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Resource/ResourceBase.h>

static bool operator<(const VkImageSubresourceRange& lhs, const VkImageSubresourceRange& rhs)
{
    return std::tie(lhs.aspectMask, lhs.baseArrayLayer, lhs.baseMipLevel, lhs.layerCount, lhs.levelCount) <
        std::tie(rhs.aspectMask, rhs.baseArrayLayer, rhs.baseMipLevel, rhs.layerCount, rhs.levelCount);
};

class VKDevice;

class VKResource : public ResourceBase
{
public:
    VKResource(VKDevice& device);

    void CommitMemory(MemoryType memoryType) override;
    void BindMemory(const ezSharedPtr<Memory>& memory, ezUInt64 offset) override;
    ezUInt64 GetWidth() const override;
    ezUInt32 GetHeight() const override;
    ezUInt16 GetLayerCount() const override;
    ezUInt16 GetLevelCount() const override;
    ezUInt32 GetSampleCount() const override;
    ezUInt64 GetAccelerationStructureHandle() const override;
    void SetName(const ezString& name) override;
    ezUInt8* Map() override;
    void Unmap() override;
    bool AllowCommonStatePromotion(ResourceState state_after) override;
    MemoryRequirements GetMemoryRequirements() const override;

    struct Image
    {
        vk::Image res;
        vk::UniqueImage res_owner;
        vk::Format format = vk::Format::eUndefined;
        vk::Extent2D size = {};
        ezUInt32 level_count = 1;
        ezUInt32 sample_count = 1;
        ezUInt32 array_layers = 1;
    } image;

    struct Buffer
    {
        vk::UniqueBuffer res;
        ezUInt32 size = 0;
    } buffer;

    struct Sampler
    {
        vk::UniqueSampler res;
    } sampler;

    vk::UniqueAccelerationStructureKHR acceleration_structure_handle = {};

private:
    VKDevice& m_device;
    vk::DeviceMemory m_vk_memory;
};
