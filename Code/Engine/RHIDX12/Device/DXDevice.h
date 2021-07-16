#pragma once
#include <RHI/Device/Device.h>
#include <RHIDX12/CPUDescriptorPool/DXCPUDescriptorPool.h>
#include <RHIDX12/GPUDescriptorPool/DXGPUDescriptorPool.h>
#include <directx/d3d12.h>
#include <dxgi.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class DXAdapter;
class DXCommandQueue;

class DXDevice : public Device
{
public:
  DXDevice(DXAdapter& adapter);

  uint32_t GetTextureDataPitchAlignment() const override;
  bool IsDxrSupported() const override;
  bool IsRayQuerySupported() const override;
  bool IsVariableRateShadingSupported() const override;
  bool IsMeshShadingSupported() const override;
  uint32_t GetShadingRateImageTileSize() const override;
  MemoryBudget GetMemoryBudget() const override;
  uint32_t GetShaderGroupHandleSize() const override;
  uint32_t GetShaderRecordAlignment() const override;
  uint32_t GetShaderTableAlignment() const override;
  RaytracingASPrebuildInfo GetBLASPrebuildInfo(const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags) const override;
  RaytracingASPrebuildInfo GetTLASPrebuildInfo(uint32_t instance_count, BuildAccelerationStructureFlags flags) const override;

  DXAdapter& GetAdapter();
  ComPtr<ID3D12Device> GetDevice();
  DXCPUDescriptorPool& GetCPUDescriptorPool();
  DXGPUDescriptorPool& GetGPUDescriptorPool();
  bool IsRenderPassesSupported() const;
  bool IsUnderGraphicsDebugger() const;
  bool IsCreateNotZeroedAvailable() const;
  ID3D12CommandSignature* GetCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE type, uint32_t stride);

protected:
  virtual ezResult InitPlatform() override { return EZ_SUCCESS; }
  virtual ezResult ShutdownPlatform() override { return EZ_SUCCESS; }

  virtual void BeginFramePlatform() override {}
  virtual void EndFramePlatform() override {}

  std::shared_ptr<Memory> AllocateMemoryPlatform(uint64_t size, MemoryType memory_type, uint32_t memory_type_bits) override;
  std::shared_ptr<CommandQueue> GetCommandQueuePlatform(CommandListType type) override;
  std::shared_ptr<Swapchain> CreateSwapchainPlatform(Window window, uint32_t width, uint32_t height, uint32_t frame_count, bool vsync) override;
  virtual void DestroySwapchainPlatform(Swapchain* swapchain) override {}
  std::shared_ptr<CommandList> CreateCommandListPlatform(CommandListType type) override;
  virtual void DestroyCommandListPlatform(CommandList* commandList) override {}
  std::shared_ptr<Fence> CreateFencePlatform(uint64_t initial_value) override;
  virtual void DestroyFencePlatform(Fence* fence) override {}

  std::shared_ptr<Resource> CreateTexturePlatform(TextureType type, uint32_t bind_flag, ezRHIResourceFormat::Enum format, uint32_t sample_count, int width, int height, int depth, int mip_levels) override;
  virtual void DestroyTexturePlatform(Resource* texture) override {}

  std::shared_ptr<Resource> CreateBufferPlatform(uint32_t bind_flag, uint32_t buffer_size) override;
  virtual void DestroyBufferPlatform(Resource* buffer) override {}

  std::shared_ptr<Resource> CreateSamplerPlatform(const SamplerDesc& desc) override;
  virtual void DestroySamplerPlatform(Resource* sampler) override {}

  std::shared_ptr<View> CreateViewPlatform(const std::shared_ptr<Resource>& resource, const ViewDesc& view_desc) override;
  virtual void DestroyViewPlatform(View* view) override {}

  std::shared_ptr<BindingSetLayout> CreateBindingSetLayoutPlatform(const std::vector<BindKey>& descs) override;
  virtual void DestroyBindingSetLayoutPlatform(BindingSetLayout* bindingSetLayout) override {}

  std::shared_ptr<BindingSet> CreateBindingSetPlatform(const std::shared_ptr<BindingSetLayout>& layout) override;
  virtual void DestroyBindingSetPlatform(BindingSet* bindingSet) override {}

  std::shared_ptr<RenderPass> CreateRenderPassPlatform(const RenderPassDesc& desc) override;
  virtual void DestroyRenderPassPlatform(RenderPass* renderPass) override {}

  std::shared_ptr<Framebuffer> CreateFramebufferPlatform(const FramebufferDesc& desc) override;
  virtual void DestroyFramebufferPlatform(Framebuffer* framebuffer) override {}

  std::shared_ptr<Shader> CreateShaderPlatform(const ShaderDesc& desc, std::vector<uint8_t> byteCode, std::shared_ptr<ShaderReflection> reflection) override;
  virtual void DestroyShaderPlatform(Shader* shader) override {}

  std::shared_ptr<Program> CreateProgramPlatform(const std::vector<std::shared_ptr<Shader>>& shaders) override;
  virtual void DestroyProgramPlatform(Program* program) override {}

  std::shared_ptr<Pipeline> CreateGraphicsPipelinePlatform(const GraphicsPipelineDesc& desc) override;
  virtual void DestroyGraphicsPipelinePlatform(Pipeline* pipeline) override {}

  std::shared_ptr<Pipeline> CreateComputePipelinePlatform(const ComputePipelineDesc& desc) override;
  virtual void DestroyComputePipelinePlatform(Pipeline* pipeline) override {}

  std::shared_ptr<Pipeline> CreateRayTracingPipelinePlatform(const RayTracingPipelineDesc& desc) override;
  virtual void DestroyRayTracingPipelinePlatform(Pipeline* pipeline) override {}

  std::shared_ptr<Resource> CreateAccelerationStructurePlatform(AccelerationStructureType type, const std::shared_ptr<Resource>& resource, uint64_t offset) override;
  virtual void DestroyAccelerationStructurePlatform(Resource* accelerationStructure) override {}

  std::shared_ptr<QueryHeap> CreateQueryHeapPlatform(QueryHeapType type, uint32_t count) override;
  virtual void DestroyQueryHeapPlatform(QueryHeap* queryHeap) override {}


private:
  RaytracingASPrebuildInfo GetAccelerationStructurePrebuildInfo(const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs) const;

  DXAdapter& m_adapter;
  ComPtr<ID3D12Device> m_device;
  ComPtr<ID3D12Device5> m_device5;
  ezMap<CommandListType, std::shared_ptr<DXCommandQueue>> m_command_queues;
  DXCPUDescriptorPool m_cpu_descriptor_pool;
  DXGPUDescriptorPool m_gpu_descriptor_pool;
  bool m_is_dxr_supported = false;
  bool m_is_ray_query_supported = false;
  bool m_is_render_passes_supported = false;
  bool m_is_variable_rate_shading_supported = false;
  bool m_is_mesh_shading_supported = false;
  uint32_t m_shading_rate_image_tile_size = 0;
  bool m_is_under_graphics_debugger = false;
  bool m_is_create_not_zeroed_available = false;
  ezMap<std::pair<D3D12_INDIRECT_ARGUMENT_TYPE, uint32_t>, ComPtr<ID3D12CommandSignature>> m_command_signature_cache;
};

D3D12_RESOURCE_STATES ConvertState(ResourceState state);
D3D12_HEAP_TYPE GetHeapType(MemoryType memory_type);
D3D12_RAYTRACING_GEOMETRY_DESC FillRaytracingGeometryDesc(const BufferDesc& vertex, const BufferDesc& index, RaytracingGeometryFlags flags);
D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS Convert(BuildAccelerationStructureFlags flags);
