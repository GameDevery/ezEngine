#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/View/View.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUDescriptorPoolRange.h>
#include <RHIVulkan/Resource/VKResource.h>

class VKDevice;

class VKView : public View
{
public:
  VKView(VKDevice& device, const ezSharedPtr<VKResource>& resource, const ViewDesc& view_desc);
  ezSharedPtr<Resource> GetResource() override;
  ezUInt32 GetDescriptorId() const override;
  ezUInt32 GetBaseMipLevel() const override;
  ezUInt32 GetLevelCount() const override;
  ezUInt32 GetBaseArrayLayer() const override;
  ezUInt32 GetLayerCount() const override;

  vk::ImageView GetImageView() const;
  vk::WriteDescriptorSet GetDescriptor() const;

private:
  void CreateView();
  void CreateImageView();
  void CreateBufferView();

  VKDevice& m_device;
  ezSharedPtr<VKResource> m_resource;
  ViewDesc m_view_desc;
  vk::UniqueImageView m_image_view;
  vk::UniqueBufferView m_buffer_view;
  std::shared_ptr<VKGPUDescriptorPoolRange> m_range;
  vk::DescriptorImageInfo m_descriptor_image = {};
  vk::DescriptorBufferInfo m_descriptor_buffer = {};
  vk::WriteDescriptorSetAccelerationStructureKHR m_descriptor_acceleration_structure = {};
  vk::WriteDescriptorSet m_descriptor = {};
};
