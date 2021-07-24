#pragma once
#include <RHIVulkan/RHIVulkanDLL.h>

#include <RHI/Device/Device.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUBindlessDescriptorPoolTyped.h>
#include <RHIVulkan/GPUDescriptorPool/VKGPUDescriptorPool.h>

class VKAdapter;
class VKCommandQueue;

class VKDevice : public Device
{
public:
  VKDevice(VKAdapter& adapter);
  ezSharedPtr<Memory> AllocateMemory(ezUInt64 size, MemoryType memoryType, ezUInt32 memoryTypeBits) override;
  ezSharedPtr<CommandQueue> GetCommandQueue(CommandListType type) override;

  ezSharedPtr<Swapchain> CreateSwapchain(Window window, ezUInt32 width, ezUInt32 height, ezUInt32 frameCount, bool vsync) override;
  void DestroySwapchain(Swapchain* pSwapChain) override {}

  ezSharedPtr<CommandList> CreateCommandList(CommandListType type) override;
  void DestroyCommandList(CommandList* pCommandList) override {}

  std::shared_ptr<Fence> CreateFence(ezUInt64 initial_value) override;
  void DestroyFence(Fence* pFence) override {}

  std::shared_ptr<Resource> CreateTexture(TextureType type, ezUInt32 bindFlags, ezRHIResourceFormat::Enum format, ezUInt32 sample_count, int width, int height, int depth, int mipLevels) override;
  void DestroyTexture(Resource* pTexture) override {}

  std::shared_ptr<Resource> CreateBuffer(ezUInt32 bind_flag, ezUInt32 buffer_size) override;
  void DestroyBuffer(Resource* pBuffer) override {}

  std::shared_ptr<Resource> CreateSampler(const SamplerDesc& desc) override;
  void DestroySampler(Resource* pSampler) override {}

  std::shared_ptr<View> CreateView(const std::shared_ptr<Resource>& resource, const ViewDesc& viewDesc) override;
  void DestroyView(View* pView) override {}

  std::shared_ptr<BindingSetLayout> CreateBindingSetLayout(const std::vector<BindKey>& descs) override;
  void DestroyBindingSetLayout(BindingSetLayout* pBindingSetLayout) override {}

  std::shared_ptr<BindingSet> CreateBindingSet(const std::shared_ptr<BindingSetLayout>& layout) override;
  void DestroyBindingSet(BindingSet* pBindingSet) override {}

  std::shared_ptr<RenderPass> CreateRenderPass(const RenderPassDesc& desc) override;
  void DestroyRenderPass(RenderPass* pRenderPass) override {}

  std::shared_ptr<Framebuffer> CreateFramebuffer(const FramebufferDesc& desc) override;
  void DestroyFramebuffer(Framebuffer* pFramebuffer) override {}

  std::shared_ptr<Shader> CreateShader(const ShaderDesc& desc, std::vector<ezUInt8> byteCode, std::shared_ptr<ShaderReflection> reflection) override;
  void DestroyShader(Shader* pShader) override {}

  std::shared_ptr<Program> CreateProgram(const std::vector<std::shared_ptr<Shader>>& shaders) override;
  void DestroyProgram(Program* pProgram) override {}

  std::shared_ptr<Pipeline> CreateGraphicsPipeline(const GraphicsPipelineDesc& desc) override;
  void DestroyGraphicsPipeline(Pipeline* pPipeline) override {}

  std::shared_ptr<Pipeline> CreateComputePipeline(const ComputePipelineDesc& desc) override;
  void DestroyComputePipeline(Pipeline* pPipeline) override {}

  std::shared_ptr<Pipeline> CreateRayTracingPipeline(const RayTracingPipelineDesc& desc) override;
  void DestroyRayTracingPipeline(Pipeline* pPipeline) override {}

  std::shared_ptr<Resource> CreateAccelerationStructure(AccelerationStructureType type, const std::shared_ptr<Resource>& resource, ezUInt64 offset) override;
  void DestroyAccelerationStructure(Resource* pAccelerationStructure) override {}

  std::shared_ptr<QueryHeap> CreateQueryHeap(QueryHeapType type, ezUInt32 count) override;
  void DestroyQueryHeap(QueryHeap* pQueryHeap) override {}

  ezUInt32 GetTextureDataPitchAlignment() const override;
  bool IsDxrSupported() const override;
  bool IsRayQuerySupported() const override;
  bool IsVariableRateShadingSupported() const override;
  bool IsMeshShadingSupported() const override;
  ezUInt32 GetShadingRateImageTileSize() const override;
  MemoryBudget GetMemoryBudget() const override;
  ezUInt32 GetShaderGroupHandleSize() const override;
  ezUInt32 GetShaderRecordAlignment() const override;
  ezUInt32 GetShaderTableAlignment() const override;
  RaytracingASPrebuildInfo GetBLASPrebuildInfo(const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags) const override;
  RaytracingASPrebuildInfo GetTLASPrebuildInfo(ezUInt32 instance_count, BuildAccelerationStructureFlags flags) const override;

  VKAdapter& GetAdapter();
  vk::Device GetDevice();
  CommandListType GetAvailableCommandListType(CommandListType type);
  vk::CommandPool GetCmdPool(CommandListType type);
  vk::ImageAspectFlags GetAspectFlags(vk::Format format) const;
  ezUniquePtr<VKGPUBindlessDescriptorPoolTyped>& GetGPUBindlessDescriptorPool(vk::DescriptorType type);
  VKGPUDescriptorPool& GetGPUDescriptorPool();
  ezUInt32 FindMemoryType(ezUInt32 type_filter, vk::MemoryPropertyFlags properties);
  vk::AccelerationStructureGeometryKHR FillRaytracingGeometryTriangles(const BufferDesc& vertex, const BufferDesc& index, RaytracingGeometryFlags flags) const;

private:
  RaytracingASPrebuildInfo GetAccelerationStructurePrebuildInfo(const vk::AccelerationStructureBuildGeometryInfoKHR& acceleration_structure_info, const std::vector<ezUInt32>& max_primitive_counts) const;

  VKAdapter& m_Adapter;
  const vk::PhysicalDevice& m_PhysicalDevice;
  vk::UniqueDevice m_Device;
  struct QueueInfo
  {
    ezUInt32 QueueFamilyIndex;
    ezUInt32 QueueCount;
  };
  ezMap<CommandListType, QueueInfo> m_QueuesInfo;
  ezMap<CommandListType, vk::UniqueCommandPool> m_CmdPools;
  ezMap<CommandListType, ezSharedPtr<VKCommandQueue>> m_CommandQueues;
  ezMap<vk::DescriptorType, ezUniquePtr<VKGPUBindlessDescriptorPoolTyped>> m_GPUBindlessDescriptorPool;
  VKGPUDescriptorPool m_GPUDescriptorPool;
  bool m_is_variable_rate_shading_supported = false;
  ezUInt32 m_shading_rate_image_tile_size = 0;
  bool m_is_dxr_supported = false;
  bool m_is_ray_query_supported = false;
  bool m_is_mesh_shading_supported = false;
  ezUInt32 m_shader_group_handle_size = 0;
  ezUInt32 m_shader_record_alignment = 0;
  ezUInt32 m_shader_table_alignment = 0;
};

vk::ImageLayout ConvertState(ResourceState state);
vk::BuildAccelerationStructureFlagsKHR Convert(BuildAccelerationStructureFlags flags);
