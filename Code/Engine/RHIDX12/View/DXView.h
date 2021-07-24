#pragma once
#include <RHI/View/View.h>
#include <RHIDX12/CPUDescriptorPool/DXCPUDescriptorHandle.h>
#include <RHIDX12/GPUDescriptorPool/DXGPUDescriptorPoolRange.h>
#include <RHIDX12/Resource/DXResource.h>

class DXDevice;
class DXResource;

class DXView : public View
{
public:
  DXView(DXDevice& device, const ezSharedPtr<DXResource>& resource, const ViewDesc& view_desc);
  ezSharedPtr<Resource> GetResource() override;
  ezUInt32 GetDescriptorId() const override;
  ezUInt32 GetBaseMipLevel() const override;
  ezUInt32 GetLevelCount() const override;
  ezUInt32 GetBaseArrayLayer() const override;
  ezUInt32 GetLayerCount() const override;

  D3D12_CPU_DESCRIPTOR_HANDLE GetHandle();

private:
  void CreateView();
  void CreateSRV();
  void CreateRAS();
  void CreateUAV();
  void CreateRTV();
  void CreateDSV();
  void CreateCBV();
  void CreateSampler();

  DXDevice& m_device;
  ezSharedPtr<DXResource> m_resource;
  ViewDesc m_view_desc;
  std::shared_ptr<DXCPUDescriptorHandle> m_handle;
  std::shared_ptr<DXGPUDescriptorPoolRange> m_range;
};
