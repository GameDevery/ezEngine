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
  ezSharedPtr<Memory> AllocateMemory(ezUInt64 size, MemoryType memory_type, ezUInt32 memory_type_bits) override;
  ezSharedPtr<CommandQueue> GetCommandQueue(CommandListType type) override;

  ezSharedPtr<Swapchain> CreateSwapchain(Window window, ezUInt32 width, ezUInt32 height, ezUInt32 frame_count, bool vsync) override;
  void DestroySwapchain(Swapchain* pSwapChain) override {}

  ezSharedPtr<CommandList> CreateCommandList(CommandListType type) override;
  void DestroyCommandList(CommandList* pCommandList) override {}

  ezSharedPtr<Fence> CreateFence(ezUInt64 initialValue) override;
  void DestroyFence(Fence* pFence) override {}

  ezSharedPtr<Resource> CreateTexture(TextureType type, ezUInt32 bindFlag, ezRHIResourceFormat::Enum format, ezUInt32 sample_count, int width, int height, int depth, int mip_levels) override;
  void DestroyTexture(Resource* pTexture) override {}

  ezSharedPtr<Resource> CreateBuffer(ezUInt32 bind_flag, ezUInt32 buffer_size) override;
  void DestroyBuffer(Resource* pBuffer) override {}

  ezSharedPtr<Resource> CreateSampler(const SamplerDesc& desc) override;
  void DestroySampler(Resource* pSampler) override {}

  ezSharedPtr<View> CreateView(const ezSharedPtr<Resource>& resource, const ViewDesc& view_desc) override;
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

  ezSharedPtr<Resource> CreateAccelerationStructure(AccelerationStructureType type, const ezSharedPtr<Resource>& resource, ezUInt64 offset) override;
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

  DXAdapter& GetAdapter();
  ComPtr<ID3D12Device> GetDevice();
  DXCPUDescriptorPool& GetCPUDescriptorPool();
  DXGPUDescriptorPool& GetGPUDescriptorPool();
  bool IsRenderPassesSupported() const;
  bool IsUnderGraphicsDebugger() const;
  bool IsCreateNotZeroedAvailable() const;
  ID3D12CommandSignature* GetCommandSignature(D3D12_INDIRECT_ARGUMENT_TYPE type, ezUInt32 stride);

private:
  RaytracingASPrebuildInfo GetAccelerationStructurePrebuildInfo(const D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS& inputs) const;

  DXAdapter& m_adapter;
  ComPtr<ID3D12Device> m_device;
  ComPtr<ID3D12Device5> m_device5;
  ezMap<CommandListType, ezSharedPtr<DXCommandQueue>> m_command_queues;
  DXCPUDescriptorPool m_cpu_descriptor_pool;
  DXGPUDescriptorPool m_gpu_descriptor_pool;
  bool m_is_dxr_supported = false;
  bool m_is_ray_query_supported = false;
  bool m_is_render_passes_supported = false;
  bool m_is_variable_rate_shading_supported = false;
  bool m_is_mesh_shading_supported = false;
  ezUInt32 m_shading_rate_image_tile_size = 0;
  bool m_is_under_graphics_debugger = false;
  bool m_is_create_not_zeroed_available = false;
  ezMap<std::pair<D3D12_INDIRECT_ARGUMENT_TYPE, ezUInt32>, ComPtr<ID3D12CommandSignature>> m_command_signature_cache;
};

D3D12_RESOURCE_STATES ConvertState(ResourceState state);
D3D12_HEAP_TYPE GetHeapType(MemoryType memory_type);
D3D12_RAYTRACING_GEOMETRY_DESC FillRaytracingGeometryDesc(const BufferDesc& vertex, const BufferDesc& index, RaytracingGeometryFlags flags);
D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS Convert(BuildAccelerationStructureFlags flags);
