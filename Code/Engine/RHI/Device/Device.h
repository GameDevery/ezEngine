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
  ezUInt64 budget;
  ezUInt64 usage;
};

class EZ_RHI_DLL Device : public QueryInterface
{
public:
  virtual ~Device() = default;
  virtual ezSharedPtr<Memory> AllocateMemory(ezUInt64 size, MemoryType memoryType, ezUInt32 memoryTypeBits) = 0;
  virtual ezSharedPtr<CommandQueue> GetCommandQueue(CommandListType type) = 0;

  virtual ezSharedPtr<Swapchain> CreateSwapchain(Window window, ezUInt32 width, ezUInt32 height, ezUInt32 frameCount, bool vsync) = 0;
  virtual void DestroySwapchain(Swapchain* pSwapChain) = 0;

  virtual ezSharedPtr<CommandList> CreateCommandList(CommandListType type) = 0;
  virtual void DestroyCommandList(CommandList* pCommandList) = 0;

  virtual ezSharedPtr<Fence> CreateFence(ezUInt64 initialValue) = 0;
  virtual void DestroyFence(Fence* pFence) = 0;

  virtual ezSharedPtr<Resource> CreateTexture(TextureType type, ezUInt32 bindFlags, ezRHIResourceFormat::Enum format, ezUInt32 sampleCount, int width, int height, int depth, int mipLevels) = 0;
  virtual void DestroyTexture(Resource* pTexture) = 0;

  virtual ezSharedPtr<Resource> CreateBuffer(ezUInt32 bindFlag, ezUInt32 bufferSize) = 0;
  virtual void DestroyBuffer(Resource* pBuffer) = 0;

  virtual ezSharedPtr<Resource> CreateSampler(const SamplerDesc& desc) = 0;
  virtual void DestroySampler(Resource* pSampler) = 0;

  virtual ezSharedPtr<View> CreateView(const ezSharedPtr<Resource>& resource, const ViewDesc& viewDesc) = 0;
  virtual void DestroyView(View* pView) = 0;

  virtual ezSharedPtr<BindingSetLayout> CreateBindingSetLayout(const std::vector<BindKey>& descs) = 0;
  virtual void DestroyBindingSetLayout(BindingSetLayout* pBindingSetLayout) = 0;

  virtual ezSharedPtr<BindingSet> CreateBindingSet(const ezSharedPtr<BindingSetLayout>& layout) = 0;
  virtual void DestroyBindingSet(BindingSet* pBindingSet) = 0;

  virtual ezSharedPtr<RenderPass> CreateRenderPass(const RenderPassDesc& desc) = 0;
  virtual void DestroyRenderPass(RenderPass* pRenderPass) = 0;

  virtual std::shared_ptr<Framebuffer> CreateFramebuffer(const FramebufferDesc& desc) = 0;
  virtual void DestroyFramebuffer(Framebuffer* pFramebuffer) = 0;

  virtual std::shared_ptr<Shader> CreateShader(const ShaderDesc& desc, ezDynamicArray<ezUInt8> byteCode, std::shared_ptr<ShaderReflection> reflection) = 0;
  virtual void DestroyShader(Shader* pShader) = 0;

  virtual std::shared_ptr<Program> CreateProgram(const std::vector<std::shared_ptr<Shader>>& shaders) = 0;
  virtual void DestroyProgram(Program* pProgram) = 0;

  virtual std::shared_ptr<Pipeline> CreateGraphicsPipeline(const GraphicsPipelineDesc& desc) = 0;
  virtual void DestroyGraphicsPipeline(Pipeline* pPipeline) = 0;

  virtual std::shared_ptr<Pipeline> CreateComputePipeline(const ComputePipelineDesc& desc) = 0;
  virtual void DestroyComputePipeline(Pipeline* pPipeline) = 0;

  virtual std::shared_ptr<Pipeline> CreateRayTracingPipeline(const RayTracingPipelineDesc& desc) = 0;
  virtual void DestroyRayTracingPipeline(Pipeline* pPipeline) = 0;

  virtual ezSharedPtr<Resource> CreateAccelerationStructure(AccelerationStructureType type, const ezSharedPtr<Resource>& resource, ezUInt64 offset) = 0;
  virtual void DestroyAccelerationStructure(Resource* pAccelerationStructure) = 0;

  virtual std::shared_ptr<QueryHeap> CreateQueryHeap(QueryHeapType type, ezUInt32 count) = 0;
  virtual void DestroyQueryHeap(QueryHeap* pQueryHeap) = 0;

  virtual ezUInt32 GetTextureDataPitchAlignment() const = 0;
  virtual bool IsDxrSupported() const = 0;
  virtual bool IsRayQuerySupported() const = 0;
  virtual bool IsVariableRateShadingSupported() const = 0;
  virtual bool IsMeshShadingSupported() const = 0;
  virtual ezUInt32 GetShadingRateImageTileSize() const = 0;
  virtual MemoryBudget GetMemoryBudget() const = 0;
  virtual ezUInt32 GetShaderGroupHandleSize() const = 0;
  virtual ezUInt32 GetShaderRecordAlignment() const = 0;
  virtual ezUInt32 GetShaderTableAlignment() const = 0;
  virtual RaytracingASPrebuildInfo GetBLASPrebuildInfo(const std::vector<RaytracingGeometryDesc>& descs, BuildAccelerationStructureFlags flags) const = 0;
  virtual RaytracingASPrebuildInfo GetTLASPrebuildInfo(ezUInt32 instanceCount, BuildAccelerationStructureFlags flags) const = 0;
};
