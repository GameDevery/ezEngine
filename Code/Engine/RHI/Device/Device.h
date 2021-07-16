#pragma once
#include <RHI/RHIDLL.h>

#include <RHI/BindingSetLayout/BindingSetLayout.h>
#include <RHI/CommandList/CommandList.h>
#include <RHI/CommandQueue/CommandQueue.h>
#include <RHI/Fence/Fence.h>
#include <RHI/Framebuffer/Framebuffer.h>
#include <RHI/Instance/BaseTypes.h>
#include <RHI/Instance/QueryInterface.h>
#include <RHI/Memory/Memory.h>
#include <RHI/Pipeline/Pipeline.h>
#include <RHI/Program/Program.h>
#include <RHI/QueryHeap/QueryHeap.h>
#include <RHI/RenderPass/RenderPass.h>
#include <RHI/Shader/Shader.h>
#include <RHI/Swapchain/Swapchain.h>
#include <memory>
#include <vector>

struct EZ_RHI_DLL MemoryBudget
{
  uint64_t budget;
  uint64_t usage;
};

class EZ_RHI_DLL Device : public QueryInterface
{
public:
  virtual ~Device();

  ezResult Init() { return InitPlatform(); }
  ezResult Shutdown() { return ShutdownPlatform(); }

  void BeginFrame() { BeginFramePlatform(); }
  void EndFrame() { EndFramePlatform(); }

  std::shared_ptr<Memory> AllocateMemory(uint64_t size, MemoryType memory_type, uint32_t memory_type_bits) { return AllocateMemoryPlatform(size, memory_type, memory_type_bits); }
  std::shared_ptr<CommandQueue> GetCommandQueue(CommandListType type) { return GetCommandQueuePlatform(type); }

  std::shared_ptr<Swapchain> CreateSwapchain(Window window, uint32_t width, uint32_t height, uint32_t frame_count, bool vsync) { return CreateSwapchainPlatform(window, width, height, frame_count, vsync); }
  void DestroySwapchain(Swapchain* swapchain) { DestroySwapchainPlatform(nullptr); }

  std::shared_ptr<CommandList> CreateCommandList(CommandListType type);
  void DestroyCommandList(std::shared_ptr<CommandList> commandList) { DestroyCommandListPlatform(nullptr); }

  std::shared_ptr<Fence> CreateFence(uint64_t initial_value) { return CreateFencePlatform(initial_value); }
  void DestroyFence(std::shared_ptr<Fence> fence) { DestroyFencePlatform(nullptr); }

  std::shared_ptr<Resource> CreateTexture(TextureType type, uint32_t bind_flag, ezRHIResourceFormat::Enum format, uint32_t sample_count, int width, int height, int depth, int mip_levels) { return CreateTexturePlatform(type, bind_flag, format, sample_count, width, height, depth, mip_levels); }
  void DestroyTexture(std::shared_ptr<Resource> texture) { DestroyTexturePlatform(nullptr); }

  std::shared_ptr<Resource> CreateBuffer(uint32_t bind_flag, uint32_t buffer_size) { return CreateBufferPlatform(bind_flag, buffer_size); }
  void DestroyBuffer(std::shared_ptr<Resource> buffer) { DestroyBufferPlatform(nullptr); }

  std::shared_ptr<Resource> CreateSampler(const SamplerDesc& desc) { return CreateSamplerPlatform(desc); }
  void DestroySampler(std::shared_ptr<Resource> sampler) { DestroySamplerPlatform(nullptr); }

  std::shared_ptr<View> CreateView(const std::shared_ptr<Resource>& resource, const ViewDesc& view_desc) { return CreateViewPlatform(resource, view_desc); }
  void DestroyView(std::shared_ptr<View> view) { DestroyViewPlatform(nullptr); }

  std::shared_ptr<BindingSetLayout> CreateBindingSetLayout(const std::vector<BindKey>& descs) { return CreateBindingSetLayoutPlatform(descs); }
  void DestroyBindingSetLayout(std::shared_ptr<BindingSetLayout> bindingSetLayout) { DestroyBindingSetLayoutPlatform(nullptr); }

  std::shared_ptr<BindingSet> CreateBindingSet(const std::shared_ptr<BindingSetLayout>& layout) { return CreateBindingSetPlatform(layout); }
  void DestroyBindingSet(std::shared_ptr<BindingSet> bindingSet) { DestroyBindingSetPlatform(nullptr); }

  std::shared_ptr<RenderPass> CreateRenderPass(const RenderPassDesc& desc) { return CreateRenderPassPlatform(desc); }
  void DestroyRenderPass(std::shared_ptr<RenderPass> renderPass) { DestroyRenderPassPlatform(nullptr); }

  std::shared_ptr<Framebuffer> CreateFramebuffer(const FramebufferDesc& desc) { return CreateFramebufferPlatform(desc); }
  void DestroyFramebuffer(std::shared_ptr<Framebuffer> framebuffer) { DestroyFramebufferPlatform(nullptr); }

  std::shared_ptr<Shader> CreateShader(const ShaderDesc& desc, std::vector<uint8_t> byteCode, std::shared_ptr<ShaderReflection> reflection) { return CreateShaderPlatform(desc, byteCode, reflection); }
  void DestroyShader(std::shared_ptr<Shader> shader) { DestroyShaderPlatform(nullptr); }

  std::shared_ptr<Program> CreateProgram(const std::vector<std::shared_ptr<Shader>>& shaders) { return CreateProgramPlatform(shaders); }
  void DestroyProgram(std::shared_ptr<Program> program) { DestroyProgramPlatform(nullptr); }

  std::shared_ptr<Pipeline> CreateGraphicsPipeline(const GraphicsPipelineDesc& desc) { return CreateGraphicsPipelinePlatform(desc); }
  void DestroyGraphicsPipeline(std::shared_ptr<Pipeline> pipeline) { DestroyGraphicsPipelinePlatform(nullptr); }

  std::shared_ptr<Pipeline> CreateComputePipeline(const ComputePipelineDesc& desc) { return CreateComputePipelinePlatform(desc); }
  void DestroyComputePipeline(std::shared_ptr<Pipeline> pipeline) { DestroyComputePipelinePlatform(nullptr); }

  std::shared_ptr<Pipeline> CreateRayTracingPipeline(const RayTracingPipelineDesc& desc) { return CreateRayTracingPipelinePlatform(desc); }
  void DestroyRayTracingPipeline(std::shared_ptr<Pipeline> pipeline) { DestroyRayTracingPipelinePlatform(nullptr); }

  std::shared_ptr<Resource> CreateAccelerationStructure(AccelerationStructureType type, const std::shared_ptr<Resource>& resource, uint64_t offset) { return CreateAccelerationStructurePlatform(type, resource, offset); }
  void DestroyAccelerationStructure(std::shared_ptr<Resource> accelerationStructure) { DestroyAccelerationStructurePlatform(nullptr); }

  std::shared_ptr<QueryHeap> CreateQueryHeap(QueryHeapType type, uint32_t count) { return CreateQueryHeapPlatform(type, count); }
  void DestroyQueryHeap(std::shared_ptr<QueryHeap> queryHeap) { DestroyQueryHeapPlatform(nullptr); }


  Swapchain* CreateSwapchain(std::shared_ptr<Swapchain> hSwapChain) const { return nullptr; }
  CommandList* CreateCommandList(std::shared_ptr<CommandList> hCommandList) const { return nullptr; }
  Fence* CreateFence(std::shared_ptr<Fence> hFence) const { return nullptr; }
  Resource* CreateTexture(std::shared_ptr<Resource> htexture) const { return nullptr; }
  Resource* CreateBuffer(std::shared_ptr<Resource> hBuffer) const { return nullptr; }
  Resource* CreateSampler(std::shared_ptr<Resource> hSampler) const { return nullptr; }
  View* CreateView(std::shared_ptr<View> hView) const { return nullptr; }
  BindingSetLayout* CreateBindingSetLayout(std::shared_ptr<BindingSetLayout> hBindingSetLayout) const { return nullptr; }
  BindingSet* CreateBindingSet(std::shared_ptr<BindingSet> hBindingSet) const { return nullptr; }
  RenderPass* CreateRenderPass(std::shared_ptr<RenderPass> hRenderPass) const { return nullptr; }
  Framebuffer* CreateFramebuffer(std::shared_ptr<Framebuffer> hFramebuffer) const { return nullptr; }
  Shader* CreateShader(std::shared_ptr<Shader> hShader) const { return nullptr; }
  Program* CreateProgram(std::shared_ptr<Program> hProgram) const { return nullptr; }
  Pipeline* CreateGraphicsPipeline(std::shared_ptr<Pipeline> hGraphicsPipeline) const { return nullptr; }
  Pipeline* CreateComputePipeline(std::shared_ptr<Pipeline> hComputePipeline) const { return nullptr; }
  Pipeline* CreateRayTracingPipeline(std::shared_ptr<Pipeline> hRayTracingPipeline) const { return nullptr; }
  Resource* CreateAccelerationStructure(std::shared_ptr<Resource> hAccelerationStructure) const { return nullptr; }
  QueryHeap* GetQueryHeap(std::shared_ptr<QueryHeap> hQueryHeap) const { return nullptr; }

protected:
  virtual ezResult InitPlatform() = 0;
  virtual ezResult ShutdownPlatform() = 0;

  virtual void BeginFramePlatform() = 0;
  virtual void EndFramePlatform() = 0;

  virtual std::shared_ptr<Memory> AllocateMemoryPlatform(uint64_t size, MemoryType memory_type, uint32_t memory_type_bits) = 0;
  virtual std::shared_ptr<CommandQueue> GetCommandQueuePlatform(CommandListType type) = 0;

  virtual std::shared_ptr<Swapchain> CreateSwapchainPlatform(Window window, uint32_t width, uint32_t height, uint32_t frame_count, bool vsync) = 0;
  virtual void DestroySwapchainPlatform(Swapchain* swapchain) = 0;

  virtual std::shared_ptr<CommandList> CreateCommandListPlatform(CommandListType type) = 0;
  virtual void DestroyCommandListPlatform(CommandList* commandList) = 0;

  virtual std::shared_ptr<Fence> CreateFencePlatform(uint64_t initial_value) = 0;
  virtual void DestroyFencePlatform(Fence* fence) = 0;

  virtual std::shared_ptr<Resource> CreateTexturePlatform(TextureType type, uint32_t bind_flag, ezRHIResourceFormat::Enum format, uint32_t sample_count, int width, int height, int depth, int mip_levels) = 0;
  virtual void DestroyTexturePlatform(Resource* texture) = 0;

  virtual std::shared_ptr<Resource> CreateBufferPlatform(uint32_t bind_flag, uint32_t buffer_size) = 0;
  virtual void DestroyBufferPlatform(Resource* buffer) = 0;

  virtual std::shared_ptr<Resource> CreateSamplerPlatform(const SamplerDesc& desc) = 0;
  virtual void DestroySamplerPlatform(Resource* sampler) = 0;

  virtual std::shared_ptr<View> CreateViewPlatform(const std::shared_ptr<Resource>& resource, const ViewDesc& view_desc) = 0;
  virtual void DestroyViewPlatform(View* view) = 0;

  virtual std::shared_ptr<BindingSetLayout> CreateBindingSetLayoutPlatform(const std::vector<BindKey>& descs) = 0;
  virtual void DestroyBindingSetLayoutPlatform(BindingSetLayout* bindingSetLayout) = 0;

  virtual std::shared_ptr<BindingSet> CreateBindingSetPlatform(const std::shared_ptr<BindingSetLayout>& layout) = 0;
  virtual void DestroyBindingSetPlatform(BindingSet* bindingSet) = 0;

  virtual std::shared_ptr<RenderPass> CreateRenderPassPlatform(const RenderPassDesc& desc) = 0;
  virtual void DestroyRenderPassPlatform(RenderPass* renderPass) = 0;

  virtual std::shared_ptr<Framebuffer> CreateFramebufferPlatform(const FramebufferDesc& desc) = 0;
  virtual void DestroyFramebufferPlatform(Framebuffer* framebuffer) = 0;

  virtual std::shared_ptr<Shader> CreateShaderPlatform(const ShaderDesc& desc, std::vector<uint8_t> byteCode, std::shared_ptr<ShaderReflection> reflection) = 0;
  virtual void DestroyShaderPlatform(Shader* shader) = 0;

  virtual std::shared_ptr<Program> CreateProgramPlatform(const std::vector<std::shared_ptr<Shader>>& shaders) = 0;
  virtual void DestroyProgramPlatform(Program* program) = 0;

  virtual std::shared_ptr<Pipeline> CreateGraphicsPipelinePlatform(const GraphicsPipelineDesc& desc) = 0;
  virtual void DestroyGraphicsPipelinePlatform(Pipeline* pipeline) = 0;

  virtual std::shared_ptr<Pipeline> CreateComputePipelinePlatform(const ComputePipelineDesc& desc) = 0;
  virtual void DestroyComputePipelinePlatform(Pipeline* pipeline) = 0;

  virtual std::shared_ptr<Pipeline> CreateRayTracingPipelinePlatform(const RayTracingPipelineDesc& desc) = 0;
  virtual void DestroyRayTracingPipelinePlatform(Pipeline* pipeline) = 0;

  virtual std::shared_ptr<Resource> CreateAccelerationStructurePlatform(AccelerationStructureType type, const std::shared_ptr<Resource>& resource, uint64_t offset) = 0;
  virtual void DestroyAccelerationStructurePlatform(Resource* accelerationStructure) = 0;

  virtual std::shared_ptr<QueryHeap> CreateQueryHeapPlatform(QueryHeapType type, uint32_t count) = 0;
  virtual void DestroyQueryHeapPlatform(QueryHeap* queryHeap) = 0;

public:
  virtual uint32_t GetTextureDataPitchAlignment() const = 0;
  virtual bool IsDxrSupported() const = 0;
  virtual bool IsRayQuerySupported() const = 0;
  virtual bool IsVariableRateShadingSupported() const = 0;
  virtual bool IsMeshShadingSupported() const = 0;
  virtual uint32_t GetShadingRateImageTileSize() const = 0;
  virtual MemoryBudget GetMemoryBudget() const = 0;
  virtual uint32_t GetShaderGroupHandleSize() const = 0;
  virtual uint32_t GetShaderRecordAlignment() const = 0;
  virtual uint32_t GetShaderTableAlignment() const = 0;
  virtual RaytracingASPrebuildInfo GetBLASPrebuildInfo(const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags) const = 0;
  virtual RaytracingASPrebuildInfo GetTLASPrebuildInfo(uint32_t instance_count, BuildAccelerationStructureFlags flags) const = 0;

private:
  ezMutex m_CommandListsMutex;
  ezDynamicArray<std::shared_ptr<CommandList>> m_CommandLists;
  ezDynamicArray<std::shared_ptr<CommandList>> m_FreeCommandLists;
};
