#pragma once

#include <RHI/RHIDLL.h>

#include <Foundation/Memory/CommonAllocators.h>

using Window = void*;

struct ezRHIMemoryBudget
{
  ezUInt64 m_Budget;
  ezUInt64 m_Usage;
};

class EZ_RHI_DLL ezRHIDevice : public ezRefCounted
{
public:
  ezEvent<const ezRHIDeviceEvent&> m_Events;

  virtual ezRHIMemoryBudget GetMemoryBudget() const = 0;
  const ezRHIDeviceCapabilities& GetCapabilities() const;

  ezResult Initialize();
  ezResult Shutdown();

  void BeginFrame();
  void EndFrame();

  ezRHIMemoryHandle AllocateMemory(ezUInt64 size, ezRHIMemoryType memoryType, ezUInt32 memoryTypeBits);
  void FreeMemory(ezRHIMemoryHandle hMemory);

  ezSharedPtr<ezRHICommandQueue> GetCommandQueue(ezRHICommandListType type);

  ezRHISwapChainHandle CreateSwapchain(const ezRHISwapChainCreationDescription& desc);
  void DestroySwapChain(ezRHISwapChainHandle hSwapChain);

  ezRHICommandListHandle CreateCommandList(ezRHICommandListType type);
  void DestroyCommandList(ezRHICommandListHandle hCommandList);

  ezRHIFenceHandle CreateFence(ezUInt64 initialValue);
  void DestroyFence(ezRHIFenceHandle hFence);

  ezRHIResourceHandle CreateTexture(const ezRHITextureCreationDescription& desc);
  void DestroyTexture(ezRHIResourceHandle hTexture);

  ezRHIResourceHandle CreateBuffer(const ezRHIBufferCreationDescription& desc);
  void DestroyBuffer(ezRHIResourceHandle hBuffer);

  ezRHIResourceHandle CreateSampler(const ezRHISamplerCreationDescription& desc);
  void DestroySampler(ezRHIResourceHandle hSampler);

  ezRHIResourceViewHandle CreateResourceView(ezRHIResourceHandle hResource, const ezRHIResourceViewCreationDescription& desc);
  void DestroyResourceView(ezRHIResourceViewHandle hResourceView);

  ezRHIBindingSetLayoutHandle CreateBindingSetLayout(const ezDynamicArray<ezRHIBindKeyDescription>& descs);
  void DestroyBindingSetLayout(ezRHIBindingSetLayoutHandle hBindingSetLayout);

  ezRHIBindingSetHandle CreateBindingSet(ezRHIBindingSetLayoutHandle hLayout);
  void DestroyBindingSet(ezRHIBindingSetHandle hBindingSet);

  ezRHIRenderPassHandle CreateRenderPass(const ezRHIRenderPassCreationDescription& desc);
  void DestroyRenderPass(ezRHIRenderPassHandle hRenderPass);

  ezRHIFramebufferHandle CreateFramebuffer(const ezRHIFramebufferCreationDescription& desc);
  void DestroyFramebuffer(ezRHIFramebufferHandle hFrameBuffer);

  ezRHIShaderHandle CreateShader(const ezRHIShaderCreationDescription& desc);
  void DestroyShader(ezRHIShaderHandle hShader);

  ezRHIProgramHandle CreateProgram(const ezDynamicArray<ezRHIShaderHandle>& shaders);
  void DestroyProgram(ezRHIProgramHandle hProgram);

  ezRHIPipelineHandle CreateGraphicsPipeline(const ezRHIGraphicsPipelineCreationDescription& desc);
  void DestroyGraphicsPipeline(ezRHIPipelineHandle hPipeline);

  ezRHIPipelineHandle CreateComputePipeline(const ezRHIComputePipelineCreationDescription& desc);
  void DestroyComputePipeline(ezRHIPipelineHandle hPipeline);

  ezRHIPipelineHandle CreateRayTracingPipeline(const ezRHIRayTracingPipelineCreationDescription& desc);
  void DestroyRayTracingPipeline(ezRHIPipelineHandle hPipeline);

  ezRHIResourceHandle CreateAccelerationStructure(ezRHIAccelerationStructureType type, ezRHIResourceHandle hResource, ezUInt64 offset);
  void DestroyAccelerationStructure(ezRHIResourceHandle hAccelerationStructure);

  ezRHIQueryHeapHandle CreateQueryHeap(ezRHIQueryHeapType type, ezUInt32 count);
  void DestroyQueryHeap(ezRHIQueryHeapHandle hQueryHeap);

  virtual ezRHIRayTracingASPrebuildInfo GetBLASPrebuildInfo(const ezDynamicArray<ezRHIRayTracingGeometryDescription>& descs, ezRHIBuildAccelerationStructureFlags flags) const = 0;
  virtual ezRHIRayTracingASPrebuildInfo GetTLASPrebuildInfo(ezUInt32 instanceCount, ezRHIBuildAccelerationStructureFlags flags) const = 0;

  ezRHISwapChain* GetSwapchain(ezRHISwapChainHandle hSwapChain);
  ezRHICommandList* GetCommandList(ezRHICommandListHandle hCommandList);
  ezRHIFence* GetFence(ezRHIFenceHandle hFence);
  ezRHIResource* GetResource(ezRHIResourceHandle hTexture);
  ezRHIResourceView* GetResourceView(ezRHIResourceViewHandle hResourceView);
  ezRHIBindingSetLayout* GetBindingSetLayout(ezRHIBindingSetLayoutHandle hBindingSetLayout);
  ezRHIBindingSet* GetBindingSet(ezRHIBindingSetHandle hBindingSet);
  ezRHIRenderPass* GetRenderPass(ezRHIRenderPassHandle hRenderPass);
  ezRHIFramebuffer* GetFramebuffer(ezRHIFramebufferHandle hFramebuffer);
  ezRHIShader* GetShader(ezRHIShaderHandle hShader);
  ezRHIProgram* GetProgram(ezRHIProgramHandle hProgram);
  ezRHIPipeline* GetPipeline(ezRHIPipelineHandle hPipeline);
  ezRHIResource* GetAccelerationStructure(ezRHIResourceHandle hAccelerationStructure);
  ezRHIQueryHeap* GetQueryHeap(ezRHIQueryHeapHandle hQueryHeap);

public:
  // public in case someone external needs to lock multiple operations
  mutable ezMutex m_Mutex;

protected:
  friend class ezMemoryUtils;

  ezRHIDevice();
  virtual ~ezRHIDevice() = default;


  virtual ezResult InitializePlatform() = 0;
  virtual ezResult ShutdownPlatform() = 0;

  virtual void BeginFramePlatform() = 0;
  virtual void EndFramePlatform() = 0;

  virtual ezRHIMemory* AllocateMemoryPlatform(ezUInt64 size, ezRHIMemoryType memoryType, ezUInt32 memoryTypeBits) = 0;
  virtual void FreeMemoryPlatform(ezRHIMemory* pMemory) = 0;

  virtual ezSharedPtr<ezRHICommandQueue> GetCommandQueuePlatform(ezRHICommandListType type) = 0;

  virtual ezRHISwapChain* CreateSwapchainPlatform(const ezRHISwapChainCreationDescription& desc) = 0;
  virtual void DestroySwapChainPlatform(ezRHISwapChain* pSwapChain) = 0;

  virtual ezRHICommandList* CreateCommandListPlatform(ezRHICommandListType type) = 0;
  virtual void DestroyCommandListPlatform(ezRHICommandList* pCommandList) = 0;

  virtual ezRHIFence* CreateFencePlatform(ezUInt64 initialValue) = 0;
  virtual void DestroyFencePlatform(ezRHIFence* pFence) = 0;

  virtual ezRHIResource* CreateTexturePlatform(const ezRHITextureCreationDescription& desc) = 0;
  virtual void DestroyTexturePlatform(ezRHIResource* pTexture) = 0;

  virtual ezRHIResource* CreateBufferPlatform(const ezRHIBufferCreationDescription& desc) = 0;
  virtual void DestroyBufferPlatform(ezRHIResource* pBuffer) = 0;

  virtual ezRHIResource* CreateSamplerPlatform(const ezRHISamplerCreationDescription& desc) = 0;
  virtual void DestroySamplerPlatform(ezRHIResource* pSampler) = 0;

  virtual ezRHIResourceView* CreateResourceViewPlatform(ezRHIResource* hResource, const ezRHIResourceViewCreationDescription& desc) = 0;
  virtual void DestroyResourceViewPlatform(ezRHIResourceView* pResourceView) = 0;

  virtual ezRHIBindingSetLayout* CreateBindingSetLayoutPlatform(const ezDynamicArray<ezRHIBindKeyDescription>& descs) = 0;
  virtual void DestroyBindingSetLayoutPlatform(ezRHIBindingSetLayout* pBindingSetLayout) = 0;

  virtual ezRHIBindingSet* CreateBindingSetPlatform(ezRHIBindingSetLayout* hLayout) = 0;
  virtual void DestroyBindingSetPlatform(ezRHIBindingSet* pBindingSet) = 0;

  virtual ezRHIRenderPass* CreateRenderPassPlatform(const ezRHIRenderPassCreationDescription& desc) = 0;
  virtual void DestroyRenderPassPlatform(ezRHIRenderPass* pRenderPass) = 0;

  virtual ezRHIFramebuffer* CreateFramebufferPlatform(const ezRHIFramebufferCreationDescription& desc) = 0;
  virtual void DestroyFramebufferPlatform(ezRHIFramebuffer* pFrameBuffer) = 0;

  virtual ezRHIShader* CreateShaderPlatform(const ezRHIShaderCreationDescription& desc) = 0;
  virtual void DestroyShaderPlatform(ezRHIShader* pShader) = 0;

  virtual ezRHIProgram* CreateProgramPlatform(const ezDynamicArray<ezRHIShader*>& shaders) = 0;
  virtual void DestroyProgramPlatform(ezRHIProgram* pProgram) = 0;

  virtual ezRHIPipeline* CreateGraphicsPipelinePlatform(const ezRHIGraphicsPipelineCreationDescription& desc) = 0;
  virtual void DestroyGraphicsPipelinePlatform(ezRHIPipeline* pPipeline) = 0;

  virtual ezRHIPipeline* CreateComputePipelinePlatform(const ezRHIComputePipelineCreationDescription& desc) = 0;
  virtual void DestroyComputePipelinePlatform(ezRHIPipeline* pPipeline) = 0;

  virtual ezRHIPipeline* CreateRayTracingPipelinePlatform(const ezRHIRayTracingPipelineCreationDescription& desc) = 0;
  virtual void DestroyRayTracingPipelinePlatform(ezRHIPipeline* pPipeline) = 0;

  virtual ezRHIResource* CreateAccelerationStructurePlatform(ezRHIAccelerationStructureType type, ezRHIResource* hResource, ezUInt64 offset) = 0;
  virtual void DestroyAccelerationStructurePlatform(ezRHIResource* pAccelerationStructure) = 0;

  virtual ezRHIQueryHeap* CreateQueryHeapPlatform(ezRHIQueryHeapType type, ezUInt32 count) = 0;
  virtual void DestroyQueryHeapPlatform(ezRHIQueryHeap* pQueryHeap) = 0;

protected:
  ezProxyAllocator m_Allocator;
  ezLocalAllocatorWrapper m_AllocatorWrapper;
  ezRHIDeviceCapabilities m_Capabilities;

private:
  bool m_bBeginFrameCalled = false;
};
